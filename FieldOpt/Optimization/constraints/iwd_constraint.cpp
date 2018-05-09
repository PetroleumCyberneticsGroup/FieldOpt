/***********************************************************
 Copyright (C) 2017
 Mathias C. Bellout <mathias.bellout@ntnu.no>

 Created by bellout on 22/3/18.

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include "iwd_constraint.h"
#include "optimizers/SNOPTSolverC.h"

#include <FieldOpt-WellIndexCalculator/resinxx/rixx_prj_viz/RivIntersectionGeometryGenerator.h>

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// ---------------------------------------------------------
using std::cout;
using std::endl;

// =========================================================
IWDConstraint::IWDConstraint(
    Settings::Optimizer* settings,
    Model::Properties::VariablePropertyContainer *variables,
    ::Reservoir::Grid::Grid *grid,
    RICaseData *ricasedata) {

  // -------------------------------------------------------
  settings_ = settings;
  // variables_ = variables;
  grid_ = grid;
  ricasedata_ = ricasedata;

  // -------------------------------------------------------
  // print vertices for perimeter and top and bottom layer
  std::vector<cvf::Vec3d> ccp;

  // -------------------------------------------------------
  ofstream fperimeter("OLYMPUS.PERIMETER");
  fperimeter.precision(16);

  rimintersection_ = new RimIntersection(ricasedata_->mainGrid(),
                                         ricasedata_,
                                         settings);

  // Displayoffset
  // 518177.6950000525   6178154.390136719   -2278.239990234375

  // PERIMETER
  cvf::Vec3d p0 = cvf::Vec3d(527744, 6.17962e+06, -2088);
  cvf::Vec3d p1 = cvf::Vec3d(527744, 6.17962e+06, -2029);

  cvf::Vec3d p2 = cvf::Vec3d(527726, 6.18154e+06, -2029);
  cvf::Vec3d p3 = cvf::Vec3d(527726, 6.18154e+06, -2088);

  cvf::Vec3d p4 = cvf::Vec3d(522874, 6.18154e+06, -2029);
  cvf::Vec3d p5 = cvf::Vec3d(522874, 6.18154e+06, -2088);

  cvf::Vec3d p6 = cvf::Vec3d(522874, 6.17936e+06, -2029);
  cvf::Vec3d p7 = cvf::Vec3d(522874, 6.17936e+06, -2088);

  cvf::Vec3d p8 = cvf::Vec3d(527744, 6.17962e+06, -2029);
  cvf::Vec3d p9 = cvf::Vec3d(527744, 6.17962e+06, -2088);

  ccp.push_back(p0);
  ccp.push_back(p1);
  ccp.push_back(p2);
  ccp.push_back(p3);
  ccp.push_back(p4);
  ccp.push_back(p5);
  ccp.push_back(p6);
  ccp.push_back(p7);
  ccp.push_back(p8);
  ccp.push_back(p9);

  rimintersection_->appendPointToPolyLine(p0);
  rimintersection_->appendPointToPolyLine(p1);
  rimintersection_->appendPointToPolyLine(p2);
  rimintersection_->appendPointToPolyLine(p3);
  rimintersection_->appendPointToPolyLine(p4);
  rimintersection_->appendPointToPolyLine(p5);
  rimintersection_->appendPointToPolyLine(p6);
  rimintersection_->appendPointToPolyLine(p7);
  rimintersection_->appendPointToPolyLine(p8);
  rimintersection_->appendPointToPolyLine(p9);

  RivIntersectionPartMgr *imgr =
      rimintersection_->intersectionPartMgr();

  RivIntersectionGeometryGenerator *
      icsec = imgr->getCrossSectionGenerator();

  size_t vx_count = icsec->m_cellBorderLineVxes.p()->size();

//  fperimeter << icsec->m_flattenedPolylineStartPoint.x() << "   "
//             << icsec->m_flattenedPolylineStartPoint.z() << "   "
//             << icsec->m_flattenedPolylineStartPoint.x() << endl;

  cvf::Vec3d doff = ricasedata_->mainGrid()->displayModelOffset();
//  fperimeter << doff.x() << "   "
//             << doff.y() << "   "
//             << doff.z() << endl;

  for (size_t ivx = 0; ivx < vx_count; ivx++) {

    auto cblvx = icsec->m_cellBorderLineVxes.p();

    auto vx_x = cblvx->val(ivx).x() + doff.x();
    auto vx_y = cblvx->val(ivx).y() + doff.y();
    auto vx_z = cblvx->val(ivx).z() + doff.z();

    fperimeter << vx_x << "   " << vx_y << "   " << vx_z << endl;
  }

  fperimeter.close();


  // -------------------------------------------------------
  ofstream ftopbottom("OLYMPUS.TOPBOTTOM");
  ftopbottom.precision(16);

  size_t idx;

  // Loop through all cells
  for (idx = 0; idx < ricasedata_->mainGrid()->cellCount(); idx++) {

    // -----------------------------------------------------
    // get ijk index for idx cell
    size_t i, j, k;
    // rigrid_->ijkFromCellIndex(idx, &i, &j, &k);
    ricasedata_->mainGrid()->ijkFromCellIndex(idx, &i, &j, &k);
    // cout << "i:" << i << " j:" << j << " k:" << k << endl;

    // -----------------------------------------------------
    // centroid for idx cell
    //cout << ricasedata_->mainGrid()->cellCentroid(idx).x();

    // -----------------------------------------------------
    // get corners for idx cell

    //
    //     7---------6
    //    /|        /|     |k
    //   / |       / |     | /j
    //  4---------5  |     |/
    //  |  3------|--2     *---i
    //  | /       | /
    //  |/        |/
    //  0---------1

    std::array<cvf::Vec3d, 8> hc;
    ricasedata_->mainGrid()->cellCornerVertices(idx, hc.data());

    if (k == 1) {

      ftopbottom << hc[4].x() << "   " << hc[4].y() << "   " << hc[4].z() << endl;
      ftopbottom << hc[5].x() << "   " << hc[5].y() << "   " << hc[5].z() << endl;
      ftopbottom << hc[6].x() << "   " << hc[6].y() << "   " << hc[6].z() << endl;
      ftopbottom << hc[7].x() << "   " << hc[7].y() << "   " << hc[7].z() << endl;

    } else if ( k == 16) {

      ftopbottom << hc[1].x() << "   " << hc[1].y() << "   " << hc[1].z() << endl;
      ftopbottom << hc[2].x() << "   " << hc[2].y() << "   " << hc[2].z() << endl;
      ftopbottom << hc[3].x() << "   " << hc[3].y() << "   " << hc[3].z() << endl;
      ftopbottom << hc[4].x() << "   " << hc[4].y() << "   " << hc[4].z() << endl;

    }

  }

  ftopbottom.close();




  if(0) {

    // -------------------------------------------------------
    rimintersection_ = new RimIntersection(rigrid_,
                                           ricasedata_,
                                           settings);

//   95 25 -2005
//   5 55 -2005
//  cvf::Vec3d p1 = cvf::Vec3d(95, 25, -2005);
//  cvf::Vec3d p2 = cvf::Vec3d(5, 55, -2005);
//  rimintersection_->appendPointToPolyLine(p1);
//  rimintersection_->appendPointToPolyLine(p2);

//  95 35 -2005
//  35 5 -2005
//  5 35 -2005
//  35 95 -2005
//  95 35 -2005

    cvf::Vec3d p1 = cvf::Vec3d(95, 35, -2005);
    cvf::Vec3d p2 = cvf::Vec3d(35, 5, -2005);
    cvf::Vec3d p3 = cvf::Vec3d(5, 35, -2005);
    cvf::Vec3d p4 = cvf::Vec3d(35, 95, -2005);
    cvf::Vec3d p5 = cvf::Vec3d(95, 35, -2005);
    rimintersection_->appendPointToPolyLine(p1);
    rimintersection_->appendPointToPolyLine(p2);
    rimintersection_->appendPointToPolyLine(p3);
    rimintersection_->appendPointToPolyLine(p4);
    rimintersection_->appendPointToPolyLine(p5);

//  for (int ii=0; ii < 2; ++ii) {
//    cout << "Polypoint " << ii <<  ": ";
//    cout << ccc[ii].x() << " " << ccc[ii].y() << " " << ccc[ii].z() << endl;
//    // cout << "appendPointToPolyLine(ccc[" << ii << "])" << endl;
//    rimintersection_->appendPointToPolyLine(ccc[ii]);
//  }

    RivIntersectionPartMgr *imgr =
        rimintersection_->intersectionPartMgr();

    RivIntersectionGeometryGenerator *
        icsec = imgr->getCrossSectionGenerator();

    size_t vx_count = icsec->m_cellBorderLineVxes.p()->size();

    for (size_t ivx = 0; ivx < vx_count; ivx++) {
      auto vx_x = icsec->m_cellBorderLineVxes.p()->val(ivx).x();
      auto vx_y = icsec->m_cellBorderLineVxes.p()->val(ivx).y();
      auto vx_z = icsec->m_cellBorderLineVxes.p()->val(ivx).z();
      cout << "vx_x: " << vx_x << " "
           << "vx_y: " << vx_y << " "
           << "vx_z: " << vx_z << endl;
    }

    print_ri_hck_vec3f("", "", "", cvf::Vec3f::ZERO, true, false);
    for (size_t ivx = 0; ivx < vx_count; ivx++) {
      print_ri_hck_vec3f("", "", "",
                         icsec->m_cellBorderLineVxes.p()->val(ivx));
    }


    // -------------------------------------------------------
    if (settings->verb_vector()[5] > 1) // idx:5 -> mod (Model)
      cout << fstr("[mod]Init RIGrid_.", 5) << "RICell& cell" << endl;

    // size_t cellcount = rigrid_->cellCount();


  }




//  const RICell& cell = rigrid_->globalCellArray()[cellcount];
//  // size_t cellcount = ricasedata_->mainGrid()->cellCount();
//  // const RICell& cell = ricasedata_->mainGrid()->globalCellArray()[cellcount];
//
//  cout << "volume:" << cell.volume() << " count:" << cellcount<< endl;
//
//  cvf::Vec3d startp = rigrid_->cell(0).center();
//  cvf::Vec3d endp = rigrid_->cell(cellcount-1).center();
//  // cvf::Vec3d startp = ricasedata_->grid(0)->cell(0).center();
//  // cvf::Vec3d endp = ricasedata_->grid(0)->cell(cellcount-1).center();
//
//  cout << "x:" << startp.x() << " y:" << startp.y() << " z:" << startp.z() << endl;
//  cout << "x:" << endp.x() << " y:" << endp.y() << " z:" << endp.z() << endl;
//


  // -------------------------------------------------------
  // BOUNDING BOX EXPERIMENTS

  // ....................................................
//  cvf::BoundingBox bb;
//  bb.add(startp);
//  bb.add(endp);
//
//  cout << endl << fstr("[mod]bb.debugString()",5)
//       << bb.debugString().toStdString() << endl;
//  cout << fstr("bb.extent():")
//       << show_Ved3d("", bb.extent()) << endl;
//
//  // cvf::BoundingBox bbg = ricasedata_->mainGrid()->boundingBox();
//  cvf::BoundingBox bbg = rigrid_->boundingBox();
//
//  cout << fstr("[mod]bbg.debugString()",5)
//       << bbg.debugString().toStdString() << endl;
//
//  cout << fstr("bbg.extent():")
//       << show_Ved3d("", bbg.extent()) << endl;


  // ....................................................
  // FIND TIGHT GRID BB
  // Find grid extremities
//  auto gmincoord = rigrid_->minCoordinate();
//  auto gmaxcoord = rigrid_->maxCoordinate();
//
//  cvf::BoundingBox bbminmax = rigridbase_->boundingBox();
////  bbminmax.add(gmincoord);
////  bbminmax.add(gmaxcoord);
//
////  cout << fstr("[mod]rigrid_->maxCoordinate()",5)
////       << show_Ved3d("", gmaxcoord) << endl;
////
////  cout << fstr("[mod]rigrid_->minCoordinate()",5)
////       << show_Ved3d("", gmincoord) << endl;
//
//  cout << endl << fstr("[mod]bbminmax.debugString()",5)
//       << bbminmax.debugString().toStdString() << endl;
//  cout << fstr("bbminmax.extent():")
//       << show_Ved3d("", bbminmax.extent()) << endl;
//
////  for (size_t ii=0; ii < cellcount; ++ii) {
////
////

}

//==========================================================
bool IWDConstraint::CaseSatisfiesConstraint(Case *current_case) {

//  // -------------------------------------------------------
//  if (!distance_constraint_->CaseSatisfiesConstraint(c)){
//    return false;
//  }
//
//  // -------------------------------------------------------
//  for (WellSplineLength *wsl : length_constraints_) {
//    if (!wsl->CaseSatisfiesConstraint(c))
//      return false;
//  }
//
  // return true;
  return false;
}

//==========================================================
void IWDConstraint::SnapCaseToConstraints(Case *current_case) {

  // -------------------------------------------------------
  // Limit the sequential application to a max # of iterations
//  for (int i = 0; i < max_iterations_; ++i) {
//
//    // Check if case satisfies constraints
//    if (CaseSatisfiesConstraint(current_case)) {
//      return;
//    }
//    else {




  // ---------------------------------------------------
  // Apply interwell distance constraint
//      distance_constraint_->SnapCaseToConstraints(c);

  // -------------------------------------------------------
  SNOPTSolverC_ =
      new Optimization::Optimizers::SNOPTSolverC(settings_,
                                                 current_case,
                                                 grid_,
                                                 ricasedata_);

  // ---------------------------------------------------
  // Apply well length constraint to each well sequentially
//      for (WellSplineLength *wsl : length_constraints_) {
//        wsl->SnapCaseToConstraints(c);
//      }

//    } // end sequential application of iwd and length constraints

//  }

}

// ---------------------------------------------------------

}
}
