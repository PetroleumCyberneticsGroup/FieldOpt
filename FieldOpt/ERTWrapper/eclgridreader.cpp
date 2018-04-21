/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>
   Modified by M.Bellout (2017) <mathias.bellout@ntnu.no, chakibbb@gmail.com>

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

// -----------------------------------------------------------------
// STD
#include <iostream>

// -----------------------------------------------------------------
// Boost
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>

// -----------------------------------------------------------------
// FieldOpt
#include "eclgridreader.h"
#include "ertwrapper_exceptions.h"

namespace ERTWrapper {
namespace ECLGrid {

using std::vector;

Eigen::Vector3d ECLGridReader::GetCellDxDyDz(int global_index) {

  double dx, dy, dz;
  dx = ecl_grid_get_cell_dx1(ecl_grid_, global_index);
  dy = ecl_grid_get_cell_dy1(ecl_grid_, global_index);
  dz = ecl_grid_get_cell_dz1(ecl_grid_, global_index);

  return Eigen::Vector3d(dx, dy, dz);
}

Eigen::Vector3d ECLGridReader::GetCellCenter(int global_index) {

  double cx, cy, cz;
  ecl_grid_get_xyz1(ecl_grid_, global_index, &cx, &cy, &cz);
  return Eigen::Vector3d(cx, cy, cz);
}

vector<Eigen::Vector3d> ECLGridReader::GetCellCorners(int global_index) {

  vector<Eigen::Vector3d> corners;
  for (int i = 0; i < 8; ++i) {
    double x, y, z;
    ecl_grid_get_cell_corner_xyz1(ecl_grid_, global_index, i, &x, &y, &z);
    corners.push_back(Eigen::Vector3d(x, y, z));
  }
  return corners;
}

MatrixXd ECLGridReader::GetCellCornersM(int global_index) {
  MatrixXd corners(8,3);
  for (int i = 0; i < 8; ++i) {
    double x, y, z;
    ecl_grid_get_cell_corner_xyz1(ecl_grid_, global_index, i, &x, &y, &z);
    corners.row(i) << x, y, z;
  }
  return corners;
}

double ECLGridReader::GetCellVolume(int global_index) {
  return ecl_grid_get_cell_volume1(ecl_grid_,
                                   global_index);
}

ECLGridReader::ECLGridReader() {
  ecl_grid_ = 0;
  ecl_file_grid_ = 0;
  ecl_file_init_ = 0;
  ecl_file_egrid_ = 0;
  poro_kw_ = 0;
  permx_kw_ = 0;
  permy_kw_ = 0;
  permz_kw_ = 0;
  actnum_kw_ = 0;
  coord_kw_ = 0;
  zcorn_kw_ = 0;
}

ECLGridReader::~ECLGridReader() {
  if (ecl_grid_ != 0)
    ecl_grid_free(ecl_grid_);
  if (ecl_file_grid_ != 0)
    ecl_file_close(ecl_file_grid_);
  if (ecl_file_init_ != 0)
    ecl_file_close(ecl_file_init_);
}

void ECLGridReader::ReadEclGrid(std::string file_name) {

  file_name_ = file_name;

  /* This is a libwellindexcalculator-adgprs.so issue: ecl_file_iget_named_kw
   * cannot find ACTNUM, COORD and ZCORN in GRID file when called from ADGPRS.
   * Likely b/c ADGPRS provides GRID and not EGRID file name to the *so. This
   * is hardwired, and has been changed, but the problem persists, somehow...
   * This check assures EGRID files is always read in, regardless of input.
   */

  // Set egrid name
  egrid_file_name_ = file_name;
  if (boost::algorithm::ends_with(file_name, ".GRID")) {
    boost::replace_all(egrid_file_name_, ".GRID", ".EGRID");
  }

  // Set init name
  init_file_name_ = file_name;
  if (boost::algorithm::ends_with(file_name, ".EGRID")) {
    boost::replace_all(init_file_name_, ".EGRID", ".INIT");
  } else if (boost::algorithm::ends_with(file_name, ".GRID")) {
    boost::replace_all(init_file_name_, ".GRID", ".INIT");
  }

  // Make sure to load EGRID and not GRID!
  if (ecl_grid_ == 0) {
    ecl_grid_ = ecl_grid_alloc(egrid_file_name_.c_str());
    ecl_file_grid_ = ecl_file_open(egrid_file_name_.c_str(), 0);
  } else {
    ecl_grid_free(ecl_grid_);
    ecl_grid_ = ecl_grid_alloc(egrid_file_name_.c_str());
    ecl_file_grid_ = ecl_file_open(egrid_file_name_.c_str(), 0);
  }

  if (ecl_file_init_ == 0) {
    ecl_file_init_ = ecl_file_open(init_file_name_.c_str(), 0);
    poro_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PORO", 0);
    permx_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PERMX", 0);
    permy_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PERMY", 0);
    permz_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PERMZ", 0);
    poro_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PORO", 0);
  } else {
    ecl_file_close(ecl_file_init_);
    ecl_file_init_ = ecl_file_open(init_file_name_.c_str(), 0);
    poro_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PORO", 0);
    permx_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PERMX", 0);
    permy_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PERMY", 0);
    permz_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PERMZ", 0);
    poro_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PORO", 0);
  }

  if (ecl_file_has_kw(ecl_file_grid_, "ACTNUM")) {

    actnum_kw_ = ecl_file_iget_named_kw(ecl_file_grid_, "ACTNUM", 0);
    coord_kw_ = ecl_file_iget_named_kw(ecl_file_grid_, "COORD", 0);
    zcorn_kw_ = ecl_file_iget_named_kw(ecl_file_grid_, "ZCORN", 0);

    GetGridIndices(); // Extract and name_vs_order grid index data
    GetCOORDZCORNData(); // Get grid coord data

  } else {
    printf("%s\n", "Grid object does not contain ACTNUM, COORD, ZCORN data.\n"
        "Make sure that you have provided the EGRID and not GRID\n file name.\n");

    // Debug
    // printf("%s\n", "Current grid file contains the following keywords:\n");
    // ecl_file_fprintf_kw_list(ecl_file_grid_,stdout);
  }
}

void ECLGridReader::GetGridSummary() {
  // Provide summary information about grid
  ecl_grid_summarize(ecl_grid_);
}

int ECLGridReader::ConvertIJKToGlobalIndex(ECLGridReader::IJKIndex ijk) {
  if (ecl_grid_ == 0) throw GridNotReadException(
        "Grid must be read before IJK to global index.");
  return ecl_grid_get_global_index3(ecl_grid_, ijk.i, ijk.j, ijk.k);
}

int ECLGridReader::ConvertIJKToGlobalIndex(int i, int j, int k) {
  if (ecl_grid_ == 0) throw GridNotReadException(
        "Grid must be read before IJK to global index.");
  return ecl_grid_get_global_index3(ecl_grid_, i, j, k);
}

ECLGridReader::IJKIndex ECLGridReader::ConvertGlobalIndexToIJK(int global_index) {
  if (ecl_grid_ == 0) throw GridNotReadException(
        "Grid must be read before converting global indices to IJK.");
  int i, j, k;
  ecl_grid_get_ijk1(ecl_grid_, global_index, &i, &j, &k);
  ECLGridReader::IJKIndex ijk;
  ijk.i = i; ijk.j = j; ijk.k = k;
  return ijk;
}

int ECLGridReader::ConvertMatrixActiveIndexToGlobalIndex(int index)
{
  if (ecl_grid_ == 0) throw GridNotReadException(
        "Grid must be read before converting indices.");
  else return ecl_grid_get_global_index1A(ecl_grid_, index);
}

ECLGridReader::Dims ECLGridReader::Dimensions() {
  ECLGridReader::Dims dims;
  if (ecl_grid_ == 0) throw GridNotReadException(
        "Grid must be read before getting dimensions.");
  int x, y, z;
  ecl_grid_get_dims(ecl_grid_, &x, &y, &z, NULL);
  dims.nx = x; dims.ny = y; dims.nz = z;
  return dims;
}

void ECLGridReader::GetCOORDZCORNData() {

  // COORD
  // Keep for ref:
  // coord_kw_ = ecl_file_iget_named_kw(ecl_file_grid_, "COORD", 0);
  int coord_kw_sz = ecl_kw_get_size(coord_kw_);
  gridData_.coord.resize(coord_kw_sz, 1);
  gridData_.coord.fill(0);

  for (int ii = 0; ii < coord_kw_sz; ++ii) {
    gridData_.coord(ii) = ecl_kw_iget_as_double(coord_kw_, ii);
  }

  // ZCORN
  // Keep for ref:
  // zcorn_kw_ = ecl_file_iget_named_kw(ecl_file_grid_, "ZCORN", 0);
  int zcorn_sz = ecl_grid_get_zcorn_size(ecl_grid_);
  gridData_.zcorn.resize(zcorn_sz, 1);
  gridData_.zcorn.fill(0);

  for (int ii = 0; ii < zcorn_sz; ++ii) {
    gridData_.zcorn(ii) = ecl_kw_iget_as_double(zcorn_kw_, ii);
  }

  gridData_.zcorn_rxryrz.resize(zcorn_sz, 1);
  gridData_.zcorn_rxryrz.fill(0);

}

void ECLGridReader::GetGridIndices() {
  ECLGridReader::Gidx gidx;
  if (ecl_grid_ == 0) throw GridNotReadException(
        "Grid must be read before getting grid indices.");

  // Get total number of cells, # of active cells in grid
  gidx.n_total = ecl_grid_get_global_size(ecl_grid_);
  gidx.n_active = ecl_grid_get_nactive(ecl_grid_);

  // Allocated actnum/active vectors
  gidx.dat_actnum.resize(gidx.n_total, 1);
  gidx.dat_actnum.fill(-1);
  gidx.idx_actnum.resize(gidx.n_total, 1);
  gidx.idx_actnum.fill(0);
  gidx.idx_active.resize(gidx.n_active, 1);
  gidx.idx_active.fill(-1);

  // Get actnum data, set actnum indices
  int a_idx = 0;
  // Keep for ref:
  // actnum_kw_ = ecl_file_iget_named_kw(ecl_file_grid_, "ACTNUM", 0);
  for (int g_idx = 0; g_idx < gidx.n_total; ++g_idx) {
    gidx.dat_actnum(g_idx) = ecl_kw_iget_as_double(actnum_kw_, g_idx);

    if (gidx.dat_actnum(g_idx) > 0) {
      gidx.idx_actnum(g_idx) = g_idx;
      gidx.idx_active(a_idx) = g_idx;
      ++a_idx;
    }
  }
  gidx_ = gidx;
}

int ECLGridReader::NumActiveMatrixCells()
{
  if (ecl_grid_ == 0) throw GridNotReadException("Grid must be read before getting the number of active cells.");
  else return ecl_grid_get_nactive(ecl_grid_);
}

int ECLGridReader::NumActiveFractureCells()
{
  if (ecl_grid_ == 0) throw GridNotReadException("Grid must be read before getting the number of active cells.");
  else return ecl_grid_get_nactive_fracture(ecl_grid_);
}

bool ECLGridReader::IsCellActive(int global_index)
{
  if (ecl_grid_ == 0) throw GridNotReadException("Grid must be read before getting the active status of cells.");
  else return (IsCellMatrixActive(global_index) || IsCellFractureActive(global_index));
}

bool ECLGridReader::IsCellMatrixActive(int global_index)
{
  if (ecl_grid_ == 0) throw GridNotReadException("Grid must be read before getting the active status of cells.");
  else
  {
    int active_matrix_index = ecl_grid_get_active_index1(ecl_grid_, global_index);
    if (active_matrix_index < 0) return false;
    else return true;
  }
}

bool ECLGridReader::IsCellFractureActive(int global_index)
{
  if (ecl_grid_ == 0) throw GridNotReadException("Grid must be read before getting the active status of cells.");
  else
  {
    int active_fracture_index = ecl_grid_get_active_fracture_index1(ecl_grid_, global_index);
    if (active_fracture_index < 0) return false;
    else return true;
  }
}

ECLGridReader::Cell ECLGridReader::GetGridCell(int global_index) {
  if (!GlobalIndexIsInsideGrid(global_index)) {
    throw InvalidIndexException(
        "The global index "
            + boost::lexical_cast<std::string>(global_index)
            + " is outside the grid.");
  }
  if (ecl_grid_ == 0) {
    throw GridNotReadException(
        "Grid must be read before getting grid cells.");
  }

  ECLGridReader::Cell cell;
  cell.global_index = global_index;
  cell.volume = GetCellVolume(global_index);
  cell.corners = GetCellCorners(global_index);
  cell.center = GetCellCenter(global_index);
  cell.dxdydz = GetCellDxDyDz(global_index);
  cell.matrix_active = IsCellMatrixActive(global_index);
  cell.fracture_active = IsCellFractureActive(global_index);

  // Get properties from the INIT file - only possible if the cell is active
  // Matrix grid
  int active_index = ecl_grid_get_active_index1(ecl_grid_, global_index);
  if (active_index >= 0)
  {
    cell.porosity.push_back(ecl_kw_iget_as_double(poro_kw_, active_index));
    cell.permx.push_back(ecl_kw_iget_as_double(permx_kw_, active_index));
    cell.permy.push_back(ecl_kw_iget_as_double(permy_kw_, active_index));
    cell.permz.push_back(ecl_kw_iget_as_double(permz_kw_, active_index));
  }

  // Fracture grid
  active_index = NumActiveMatrixCells() + ecl_grid_get_active_fracture_index1(ecl_grid_, global_index);
  if (active_index >= NumActiveMatrixCells())
  {
    cell.porosity.push_back(ecl_kw_iget_as_double(poro_kw_, active_index));
    cell.permx.push_back(ecl_kw_iget_as_double(permx_kw_, active_index));
    cell.permy.push_back(ecl_kw_iget_as_double(permy_kw_, active_index));
    cell.permz.push_back(ecl_kw_iget_as_double(permz_kw_, active_index));
  }

  return cell;
}

int ECLGridReader::GlobalIndexOfCellEnvelopingPoint(double x, double y, double z,
                                                    int initial_guess) {
  if (ecl_grid_ == 0) throw GridNotReadException(
        "Grid must be read before searching for cells.");
  return ecl_grid_get_global_index_from_xyz(ecl_grid_, x, y, z, initial_guess);
}

bool ECLGridReader::GlobalIndexIsInsideGrid(int global_index) {
  Dims dims = Dimensions();
  return global_index < dims.nx * dims.ny * dims.nz;
}

ECLGridReader::Cell ECLGridReader::FindSmallestCell() {
  auto dims = Dimensions();
  int max_index = dims.nx * dims.ny * dims.nz;
  int index_with_smallest_volume = 0;
  double smallest_volume = 1e7;
  double volume;
  for (int global_index = 0; global_index < max_index; ++global_index) {
    if (IsCellActive(global_index)) {
      volume = GetCellVolume(global_index);
      if (volume < smallest_volume) {
        index_with_smallest_volume = global_index;
        smallest_volume = volume;
      }
    }
  }
  return GetGridCell(index_with_smallest_volume);
}

}
}
