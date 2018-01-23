/******************************************************************************
   Copyright (C) 2017 Mathias Bellout [Created on 20170729]
   <mathias.bellout@ntnu.no, chakibbb@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "eclgridrotator.h"

using namespace ERTWrapper::ECLGrid;
using boost::property_tree::ptree;
using boost::filesystem::path;

using namespace std;
using namespace Eigen;

namespace ERTWrapper {
namespace ECLGrid {

ECLGridRotator::ECLGridRotator() {
    ecl_grid_reader_ = new ECLGridReader();
}

ECLGridRotator::~ECLGridRotator() {
    delete ecl_grid_reader_;
}

void ECLGridRotator::PrintCoordZcornData() {

    MatrixXd coord_rxryrz = ecl_grid_reader_->gridData_.coord_rxryrz;
    MatrixXd zcorn_rxryrz = ecl_grid_reader_->gridData_.zcorn_rxryrz;

    // Reshape COORD
    Map<Matrix<double, Dynamic, Dynamic>, Unaligned, Stride<1, 6>>
        coord_rxryrz_out(coord_rxryrz.data(), coord_rxryrz.size()/6, 6);
    cout << "coord_rxryrz_out.size(): " << coord_rxryrz_out.size() << endl;

    // Stream out COORD :: set up file
    ofstream streamoutfile;
    streamoutfile.open(OUTPUT_FILE_PATH, ios::out);
    streamoutfile << "COORD" << endl;

    // Stream out COORD :: set up format
    streamoutfile.setf(ios::fixed, ios::floatfield);
    streamoutfile.precision(6);

    // Print COORD
    IOFormat COORDFrmt(StreamPrecision, 0, "   ", "\n", "", "");
    streamoutfile << coord_rxryrz_out.format(COORDFrmt);
    streamoutfile << "\n/\n\n";

    // Reshape ZCORN
    streamoutfile << "ZCORN" << endl;
    Map<Matrix<double, Dynamic, Dynamic>, Unaligned, Stride<1, 8>>
        zcorn_rxryrz_out(zcorn_rxryrz.data(), zcorn_rxryrz.size()/8, 8);
    cout << "zcorn_rxryrz_out.size(): " << zcorn_rxryrz_out.size() << endl;

    IOFormat ZCORNFrmt(StreamPrecision, 0, "   ", "\n", "", "");
    streamoutfile << zcorn_rxryrz_out.format(ZCORNFrmt);
    streamoutfile << "\n/\n\n";
    streamoutfile.close();

}

// Adapted from ERT function ecl_grid_init_zcorn_data__ (eclgrid.c line 6319)
// -- see original code et end of this file (keep for reference)
void ECLGridRotator::RotateZCORN(bool rotateGrid) {

    ECLGridReader::Dims dims = ecl_grid_reader_->Dimensions();
    int nx = dims.nx;
    int ny = dims.ny;
    int nz = dims.nz;

    auto rcenter = RCENTER.replicate(1,8).transpose();
    auto roffset = ROFFSET.replicate(1,8).transpose();

    int i, j, k;
    for (j=0; j < ny; j++) {
        for (i=0; i < nx; i++) {
            for (k=0; k < nz; k++) {

                int global_cell_idx = ecl_grid_reader_->ConvertIJKToGlobalIndex(i, j, k);
                MatrixXd cornersM = ecl_grid_reader_->GetCellCornersM(global_cell_idx);

                if (rotateGrid) {
                    cornersM = (cornersM - rcenter) * Rx_ * Ry_ * Rz_ + rcenter + roffset;
                }

                int l;

                for (l=0; l < 2; l++) {
                    Vector3d p0 = cornersM.row( 4*l );
                    Vector3d p1 = cornersM.row( 4*l + 1 );
                    Vector3d p2 = cornersM.row( 4*l + 2 );
                    Vector3d p3 = cornersM.row( 4*l + 3 );

                    int z1 = k*8*nx*ny + j*4*nx + 2*i            + l*4*nx*ny;
                    int z2 = k*8*nx*ny + j*4*nx + 2*i  +  1      + l*4*nx*ny;
                    int z3 = k*8*nx*ny + j*4*nx + 2*nx + 2*i     + l*4*nx*ny;
                    int z4 = k*8*nx*ny + j*4*nx + 2*nx + 2*i + 1 + l*4*nx*ny;

                    ecl_grid_reader_->gridData_.zcorn_rxryrz(z1) = p0(2);
                    ecl_grid_reader_->gridData_.zcorn_rxryrz(z2) = p1(2);
                    ecl_grid_reader_->gridData_.zcorn_rxryrz(z3) = p2(2);
                    ecl_grid_reader_->gridData_.zcorn_rxryrz(z4) = p3(2);
                }
            }
        }
    }
}

void ECLGridRotator::RotateCOORD() {

    // cout << CYAN << "Transpose ERT vector into [x,y,z] columns" << END << endl;
    Map<MatrixXd, Unaligned, Stride<1,3>>
        coord_xyz(ecl_grid_reader_->gridData_.coord.data(),
                  ecl_grid_reader_->gridData_.coord.rows()/3, 3);
    ecl_grid_reader_->gridData_.coord_xyz = coord_xyz;

    // cout << CYAN << "Set center coordinates for rotation" << END << endl;
    if(ROTATION_MODE=="IJK") {

        int center_idx = ecl_grid_reader_->ConvertIJKToGlobalIndex(
            (int)RCENTER(0)-1, (int)RCENTER(1)-1, (int)RCENTER(2)-1);
        MatrixXd center_corners = ecl_grid_reader_->GetCellCornersM(center_idx);
        RCENTER = center_corners.row(0).transpose();

    } else if(ROTATION_MODE=="XYZ") {
        // Already read-in from json
    } else if(ROTATION_MODE=="NONE" || ROTATION_MODE=="") {
        RCENTER.fill(0);
    }

    try {
        cout << CYAN << "CENTER COORDINATE: " << END
            << RCENTER.transpose() << endl;
    } catch (const std::exception& e) {
        cout << "Error: " << e.what() << endl;
        cout << "RCENTER.rows() = " << RCENTER.rows() << endl;
    }

    // cout << CYAN << "Resize center and offset vectors" << END << endl;
    auto rcenter = RCENTER.replicate(1,coord_xyz.rows()).transpose();
    auto roffset = ROFFSET.replicate(1,coord_xyz.rows()).transpose();

    // Multiply with rotation matrices
    if (Rx_.rows() > 0 && Ry_.rows() > 0 && Rz_.rows() > 0) {

        ecl_grid_reader_->gridData_.coord_rxryrz_xyz =
            (coord_xyz - rcenter) * Rx_ * Ry_ * Rz_ + rcenter + roffset;
        cout << "Rotation performed." << endl;

    } else {
        cout << "Rotation matrices empty. No rotation performed." << endl
             << "Run GetCOORDZCORNData() b/f calling RotateCOORD()." << endl;
        ecl_grid_reader_->gridData_.coord_rxryrz_xyz = coord_xyz;
    }

    // cout << CYAN << "Reshape back into vector form" << END << endl;
    Map<MatrixXd> coord(
        ecl_grid_reader_->gridData_.coord_rxryrz_xyz.data(),
        ecl_grid_reader_->gridData_.coord.rows(), 1);
    ecl_grid_reader_->gridData_.coord_rxryrz = coord;
}

void ECLGridRotator::SetRotationMatrix(MatrixXd rx_ry_rz, bool dbg) {

    double ax = rx_ry_rz(0);
    double ay = rx_ry_rz(1);
    double az = rx_ry_rz(2);

    Rx_ << 1, 0, 0,
        0, cos(ax), -sin(ax),
        0, sin(ax), cos(ax);

    Ry_ << cos(ay), 0, sin(ay),
        0, 1, 0,
        -sin(ay), 0, cos(ay);

    Rz_ << cos(az), -sin(az), 0,
        sin(az), cos(az), 0,
        0, 0, 1;

    if (dbg){
        cout.setf(ios::fixed, ios::floatfield);
        cout.precision(6);
        cout << CYAN << "Rx: " << END << endl << Rx_ << endl
             << CYAN << "Ry: " << END << endl << Ry_ << endl
             << CYAN << "Rz: " << END << endl << Rz_ << endl
             << endl;
    }
}

void ECLGridRotator::GetParametersFromJSON(){
    int argc_dummy = 0;
    const char * argv_dummy[0];
    ECLGridRotator::GetParametersFromJSON(argc_dummy, argv_dummy);
}

void ECLGridRotator::GetParametersFromJSON(int argc, const char **argv){

    // Get input file name
    if (argc >= 2){
        param_file_ = argv[1]; // char
    }else{
        cout << "No parameter file passed as argument. " << endl;
        cout << "Correct usage is: grid_rotator /path/to/params-grid-rotation.json" << endl;

        param_file_ = "../examples/ECLIPSE/5spot_exp/5spot-params-grid-rotation.json";
        cout << "Using input parameters file in current folder (if present):\n"
             << param_file_ << endl;
    }

    // Read json parameter file
    ptree pt;
    read_json(param_file_, pt);
    path param_path(param_file_);

    // Load path of output file containing COORD and ZCORN
    output_file_ = pt.get<string>(OUTPUTFILE);
    OUTPUT_FILE_PATH = param_path.parent_path().string()
        + "/" + output_file_;
    cout << CYAN << OUTPUTFILE << ": " << END
         << OUTPUT_FILE_PATH << endl;

    // Load EGRID path: looks for the grid file in the same
    // directory as the parameter file
    grid_file_ = pt.get<string>(GRIDPATH);
    ECL_GRID_FILE_PATH = param_path.parent_path().string()
        + "/" + grid_file_;
    cout << CYAN << GRIDPATH << ": " << END
        << ECL_GRID_FILE_PATH << endl;

    // Get path and stem of output file
    path grid_path(grid_file_);

    // Load rotation angles
    vector<double> ra, va;
    BOOST_FOREACH(ptree::value_type &va, pt.get_child(ROTANGLE)) {
        ra.push_back(va.second.get_value<double>());
    }
    Map<MatrixXd> rx_ry_rz(ra.data(), ra.size(), 1);
    RX_RY_RZ = rx_ry_rz;
    cout << CYAN << ROTANGLE << ": " << END
         << RX_RY_RZ.transpose() << endl;

    // Load rotation origo option
    ROTATION_MODE = pt.get<string>(ROTMODE);

    // Inform about rotation center option (re-set rotation
    // center after eclgrid has been defined)
    cout << CYAN << "ROTATION CENTER OPTION: " << END
         << ROTATION_MODE << endl;

    // Load rotation center
    vector<double> rc, vc;
    BOOST_FOREACH(ptree::value_type &vc, pt.get_child(ROTCENTER)) {
                    rc.push_back(vc.second.get_value<double>());
                }
    Map<MatrixXd> rcenter(rc.data(), rc.size(), 1);
    RCENTER = rcenter;

    cout << CYAN << "CENTER COORDINATE: " << END
         << RCENTER.transpose() << endl;

    // Load offset
    vector<double> ro, vo;
    BOOST_FOREACH(ptree::value_type &vo, pt.get_child(ROTOFFSET)) {
                    ro.push_back(vo.second.get_value<double>());
                }
    Map<MatrixXd> roffset(ro.data(), ro.size(), 1);
    ROFFSET = roffset;

    cout << CYAN << "OFFSET COORDINATE: " << END
         << ROFFSET.transpose() << endl;

    // Set rotation matrix
    SetRotationMatrix(RX_RY_RZ);
}

}
}


// ERT function ecl_grid_init_zcorn_data__ (eclgrid.c line 6319)
//static void ecl_grid_init_zcorn_data__(const ecl_grid_type * grid,
//                                       float * zcorn_float,
//                                       double * zcorn_double) {
//  int nx = grid->nx;
//  int ny = grid->ny;
//  int nz = grid->nz;
//  int i,j,k;
//  for (j=0; j < ny; j++) {
//    for (i=0; i < nx; i++) {
//      for (k=0; k < nz; k++) {
//        const int cell_index   = ecl_grid_get_global_index3(grid, i,j,k);
//        const ecl_cell_type * cell = ecl_grid_get_cell(grid, cell_index );
//        int l;
//
//        for (l=0; l < 2; l++) {
//          point_type p0 = cell->corner_list[ 4*l];
//          point_type p1 = cell->corner_list[ 4*l + 1];
//          point_type p2 = cell->corner_list[ 4*l + 2];
//          point_type p3 = cell->corner_list[ 4*l + 3];
//
//          int z1 = k*8*nx*ny + j*4*nx + 2*i            + l*4*nx*ny;
//          int z2 = k*8*nx*ny + j*4*nx + 2*i  +  1      + l*4*nx*ny;
//          int z3 = k*8*nx*ny + j*4*nx + 2*nx + 2*i     + l*4*nx*ny;
//          int z4 = k*8*nx*ny + j*4*nx + 2*nx + 2*i + 1 + l*4*nx*ny;
//
//          if (zcorn_float) {
//            zcorn_float[z1] = p0.z;
//            zcorn_float[z2] = p1.z;
//            zcorn_float[z3] = p2.z;
//            zcorn_float[z4] = p3.z;
//          }
//
//          if (zcorn_double) {
//            zcorn_double[z1] = p0.z;
//            zcorn_double[z2] = p1.z;
//            zcorn_double[z3] = p2.z;
//            zcorn_double[z4] = p3.z;
//          }
//        }
//      }
//    }
//  }
//}
