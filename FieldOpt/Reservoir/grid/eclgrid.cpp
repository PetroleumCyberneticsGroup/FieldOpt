/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>

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

#include "eclgrid.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>

#include <FieldOpt-WellIndexCalculator/wellindexcalculator.h>
#include "FieldOpt-WellIndexCalculator/tests/wic_debug.hpp"

namespace Reservoir {
namespace Grid {

using namespace std;

ECLGrid::ECLGrid(string file_path)
    : Grid(GridSourceType::ECLIPSE, file_path) {

  if (!boost::filesystem::exists(file_path))
    throw runtime_error("Grid file " + file_path + " not found.");

  string init_file_path = file_path;
  if (boost::algorithm::ends_with(file_path, ".EGRID")) {
    boost::replace_all(init_file_path, ".EGRID", ".INIT");
  } else if (boost::algorithm::ends_with(file_path, ".GRID")) {
    boost::replace_all(init_file_path, ".GRID", ".INIT");
  }

  if (!boost::filesystem::exists(init_file_path)) {
    throw runtime_error("ECLGrid::ECLGrid: Reservoir init file "
                            + init_file_path + " not found.");
  }

  ecl_grid_reader_ = new ERTWrapper::ECLGrid::ECLGridReader();
  ecl_grid_reader_->ReadEclGrid(file_path_);

  // Calculate the proper corner permutation for cell faces definition:
  // This is a function of the z axis orientation.
  // Somehow the grid reader it re-aranging the cell corners and I could
  // not easily found a logic so we are going to check all known permutations
  // with the toe that one of them is suitable for the current grid - we do
  // that based on the cell 0 in the grid

  // Find the first (active) cell index in the matrix.
  int idx = ecl_grid_reader_->ConvertMatrixActiveIndexToGlobalIndex(0);

  // Set faces permutation to first permutation type
  faces_permutation_index_ = 0;
  // Get the first cell
  Cell first_cell = GetCell(idx);

  if (first_cell.EnvelopsPoint(first_cell.center()))
  {
    return;
  }

  // Set faces permutation to second permutation type
  faces_permutation_index_ = 1;
  // Get the first cell
  first_cell = GetCell(idx);
  if (first_cell.EnvelopsPoint(first_cell.center()))
  {
    return;
  }

  // We should not have gotten here - if here then it
  // means there we need more permutations schemes
  throw runtime_error("Unknown axis orientation");
}

ECLGrid::~ECLGrid() {
  delete ecl_grid_reader_;
}

bool ECLGrid::IndexIsInsideGrid(int global_index) {
  return global_index >= 0
      && global_index < (Dimensions().nx * Dimensions().ny * Dimensions().nz);
}

bool ECLGrid::IndexIsInsideGrid(int i, int j, int k) {
  return i >= 0 && i < Dimensions().nx &&
      j >= 0 && j < Dimensions().ny &&
      k >= 0 && k < Dimensions().nz;
}

bool ECLGrid::IndexIsInsideGrid(IJKCoordinate *ijk) {
  return IndexIsInsideGrid(ijk->i(), ijk->j(), ijk->k());
}

Grid::Dims ECLGrid::Dimensions() {
  Dims dims;
  if (type_ == GridSourceType::ECLIPSE) {
    auto eclDims = ecl_grid_reader_->Dimensions();
    dims.nx = eclDims.nx;
    dims.ny = eclDims.ny;
    dims.nz = eclDims.nz;
    return dims;
  } else {
    throw runtime_error("ECLGrid::Dimensions(): Grid "
                            "source must be defined before "
                            "getting grid dimensions.");
  }
}

Cell ECLGrid::GetCell(int global_index) {

  // Check if global index cell is inside overall (i.e., active+inactive) grid
  if (!IndexIsInsideGrid(global_index)) {
    throw runtime_error("ECLGrid::GetCell(int global_index): Error getting "
                            "grid cell. Global index is outside grid.");
  }

  if (type_ == GridSourceType::ECLIPSE) {
    auto ertCell = ecl_grid_reader_->GetGridCell(global_index);

    // Get IJK index corresponding to global index
    auto ecl_ijk_index = ecl_grid_reader_->ConvertGlobalIndexToIJK(global_index);
    IJKCoordinate ijk_index = IJKCoordinate(ecl_ijk_index.i,
                                            ecl_ijk_index.j,
                                            ecl_ijk_index.k);

    // Get cell center coordinates
    auto center = Eigen::Vector3d(ertCell.center);

    // Get cell corners
    vector<Eigen::Vector3d> corners;
    for (auto corner : ertCell.corners) {
      corners.push_back(corner);
    }

    // Return cell info
    return Cell(global_index, ijk_index,
                ertCell.volume, ertCell.porosity,
                ertCell.permx, ertCell.permy, ertCell.permz,
                center, corners, faces_permutation_index_,
                ertCell.matrix_active, ertCell.fracture_active,
                Dimensions().nz + ijk_index.k()
    );
  } else {
    throw runtime_error("ECLGrid::GetCell(int global_index): Grid "
                            "source must be defined before getting a cell.");
  }
}

Cell ECLGrid::GetCell(int i, int j, int k) {
  // Check if IJK cell is inside overall (i.e., active+inactive) grid
  if (!IndexIsInsideGrid(i, j, k)) {
    throw runtime_error("ECLGrid::GetCell(int i, int j, int k): Error "
                            "getting grid cell. Index (i, j, k) is outside "
                            "grid.");
  }

  if (type_ == GridSourceType::ECLIPSE) {
    int global_index = ecl_grid_reader_->ConvertIJKToGlobalIndex(i, j, k);
    return GetCell(global_index);
  } else {
    throw runtime_error("ECLGrid::GetCell(int i, int j, int k): Grid "
                            "source must be defined before getting a cell.");
  }
}

Cell ECLGrid::GetCell(IJKCoordinate *ijk) {
  // Check if IJK cell is inside overall (i.e., active+inactive) grid
  if (!IndexIsInsideGrid(ijk)) {
    throw runtime_error("ECLGrid::GetCell(*ijk): Error getting grid "
                            "cell. Index (i, j, k) is outside grid.");
  }

  if (type_ == GridSourceType::ECLIPSE) {
    int global_index = ecl_grid_reader_->ConvertIJKToGlobalIndex(ijk->i(), ijk->j(), ijk->k());
    return GetCell(global_index);
  } else {
    throw runtime_error("ECLGrid::GetCell(*ijk): Grid source must "
                            "be defined before getting a cell.");
  }
}

vector<int> ECLGrid::GetBoundingBoxCellIndices(
    double xi, double yi, double zi,
    double xf, double yf, double zf,
    double &bb_xi, double &bb_yi, double &bb_zi,
    double &bb_xf, double &bb_yf, double &bb_zf)

{
  double x_i, y_i, z_i, x_f, y_f, z_f;
  x_i = min(xi, xf);
  x_f = max(xi, xf);
  y_i = min(yi, yf);
  y_f = max(yi, yf);
  z_i = min(zi, zf);
  z_f = max(zi, zf);

  bb_xi = numeric_limits<double>::max();
  bb_yi = numeric_limits<double>::max();
  bb_zi = numeric_limits<double>::max();
  bb_xf = numeric_limits<double>::lowest();
  bb_yf = numeric_limits<double>::lowest();
  bb_zf = numeric_limits<double>::lowest();

  int total_cells =
      Dimensions().nx *
          Dimensions().ny *
          Dimensions().nz;

  vector<int> indices_list;
  for (int ii = 0; ii < total_cells; ii++) {
    // Try is here because we only want to get the list of active
    // cells - that means defined cells
    try {
      Cell cell = GetCell(ii);

      // Calculate cell size
      double dx = (cell.corners()[5] - cell.corners()[4]).norm();
      double dy = (cell.corners()[6] - cell.corners()[4]).norm();
      double dz = (cell.corners()[0] - cell.corners()[4]).norm();

      // Test if cell dims (dx,dy,dz) are within bbox corners
      if ((cell.center().x() >= x_i - dx/1.7) && (cell.center().x() <= x_f + dx/1.7) &&
          (cell.center().y() >= y_i - dy/1.7) && (cell.center().y() <= y_f + dy/1.7) &&
          (cell.center().z() >= z_i - dz/1.7) && (cell.center().z() <= z_f + dz/1.7)) {

        indices_list.push_back(ii);
        // Adjust bbox corners
        bb_xi = min(bb_xi, cell.center().x() - dx/2.0);
        bb_yi = min(bb_yi, cell.center().y() - dy/2.0);
        bb_zi = min(bb_zi, cell.center().z() - dz/2.0);
        bb_xf = max(bb_xf, cell.center().x() + dx/2.0);
        bb_yf = max(bb_yf, cell.center().y() + dy/2.0);
        bb_zf = max(bb_zf, cell.center().z() + dz/2.0);
      }
    }
    catch(const std::runtime_error& e)
    {
      // We should never end up here
      std::cout << e.what() << endl;
      throw std::runtime_error("Something");
    }
  }
  return indices_list;
}

// OV: 20170709
bool ECLGrid::GetCellEnvelopingPoint(Cell& cell, double x, double y, double z){
  int total_cells = Dimensions().nx * Dimensions().ny * Dimensions().nz;

  for (int ii = 0; ii < total_cells; ii++) {
    if (GetCell(ii).EnvelopsPoint(Eigen::Vector3d(x, y, z))) {
      cell = GetCell(ii);
      return true;
    }
  }

  return false;
}

// Version that returns cell
Cell ECLGrid::GetCellEnvelopingPoint(double x, double y, double z) {
  int total_cells = Dimensions().nx * Dimensions().ny * Dimensions().nz;

  for (int ii = 0; ii < total_cells; ii++) {
    if (GetCell(ii).EnvelopsPoint(Eigen::Vector3d(x, y, z))) {
      return GetCell(ii);
    }
  }

  // Throw an exception if no cell was found
  throw runtime_error("Grid::GetCellEnvelopingPoint: The point is outside the grid ("
                          + boost::lexical_cast<std::string>(x) + ", "
                          + boost::lexical_cast<std::string>(y) + ", "
                          + boost::lexical_cast<std::string>(z) + ")"
  );
}

// OV: 20170709
bool ECLGrid::GetCellEnvelopingPoint(Cell& cell, double x, double y, double z,
                                     vector<int> search_set){

  // If the searching area is empty then search the entire grid
  if (search_set.size() == 0) {
    return GetCellEnvelopingPoint(cell, x, y, z);
  }

  for (int iCell = 0; iCell < search_set.size(); iCell++) {
    if (GetCell(search_set[iCell]).EnvelopsPoint(Eigen::Vector3d(x, y, z))) {
      cell = GetCell(search_set[iCell]);
      return true;
    }
  }

  return false;
}

// Version that returns cell
Cell ECLGrid::GetCellEnvelopingPoint(double x, double y, double z,
                                     vector<int> search_set) {

  // If the searching area is empty then search the entire grid
  if (search_set.size() == 0) {
    return GetCellEnvelopingPoint(x, y, z);
  }

  for (int iCell = 0; iCell < search_set.size(); iCell++) {
    if (GetCell(search_set[iCell]).EnvelopsPoint(Eigen::Vector3d(x, y, z))) {
      return GetCell(search_set[iCell]);
    }
  }

  // Throw an exception if no cell was found
  throw runtime_error("Grid::GetCellEnvelopingPoint: The point is outside "
                          "the searching area or even outside the grid ("
                          + boost::lexical_cast<std::string>(x) + ", "
                          + boost::lexical_cast<std::string>(y) + ", "
                          + boost::lexical_cast<std::string>(z) + ")"
  );
}

// OV: 20170709
bool ECLGrid::GetCellEnvelopingPoint(Cell& cell, Eigen::Vector3d xyz) {
  return GetCellEnvelopingPoint(cell, xyz.x(), xyz.y(), xyz.z());
}

// Version that returns cell
Cell ECLGrid::GetCellEnvelopingPoint(Eigen::Vector3d xyz) {
  return GetCellEnvelopingPoint(xyz.x(), xyz.y(), xyz.z());
}

// OV: 20170709
bool ECLGrid::GetCellEnvelopingPoint(Cell& cell, Eigen::Vector3d xyz,
                                     vector<int> search_set) {
  return GetCellEnvelopingPoint(cell, xyz.x(), xyz.y(), xyz.z(), search_set);
}

// Version that returns cell
Cell ECLGrid::GetCellEnvelopingPoint(Eigen::Vector3d xyz,
                                     vector<int> search_set) {
  return GetCellEnvelopingPoint(xyz.x(), xyz.y(), xyz.z(), search_set);
}

Cell ECLGrid::GetSmallestCell() {
  return GetCell(ecl_grid_reader_->FindSmallestCell().global_index);
}

}
}
