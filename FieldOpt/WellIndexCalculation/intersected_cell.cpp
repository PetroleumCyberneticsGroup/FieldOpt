/******************************************************************************
   Copyright (C) 2015-2016 Hilmar M. Magnusson <hilmarmag@gmail.com>
   Modified by Einar J.M. Baumann (2016) <einar.baumann@gmail.com>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>
   Modified by Einar J.M. Baumann (2017) <einar.baumann@gmail.com>

   This file and the WellIndexCalculator as a whole is part of the
   FieldOpt project. However, unlike the rest of FieldOpt, the
   WellIndexCalculator is provided under the GNU Lesser General Public
   License.

   WellIndexCalculator is free software: you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation, either version 3 of
   the License, or (at your option) any later version.

   WellIndexCalculator is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with WellIndexCalculator.  If not, see
   <http://www.gnu.org/licenses/>.
******************************************************************************/
#include "intersected_cell.h"
#include <iostream>
#include <stdexcept>

using namespace std;

namespace Reservoir {
namespace WellIndexCalculation {


IntersectedCell::IntersectedCell(const Grid::Cell &cell) : Grid::Cell(cell) {
  i_ = ijk_index().i();
  j_ = ijk_index().j();
  k_ = ijk_index().k();
};

IntersectedCell::IntersectedCell() {
  i_ = ijk_index().i();
  j_ = ijk_index().j();
  k_ = ijk_index().k();
}

Vector3d IntersectedCell::xvec() const {
  return corners()[5] - corners()[4];
}

Vector3d IntersectedCell::yvec() const {
  return corners()[6] - corners()[4];
}

Vector3d IntersectedCell::zvec() const {
  return corners()[0] - corners()[4];
}


Vector3d IntersectedCell::get_segment_entry_point(int segment_index) const {
  return entry_points_[segment_index];
}

Vector3d IntersectedCell::get_segment_exit_point(int segment_index) const {
  return exit_points_[segment_index];
}

double IntersectedCell::get_segment_radius(int segment_index) const {
  return segment_radius_[segment_index];
}

double IntersectedCell::get_segment_skin(int segment_index) const {
  return segment_skin_[segment_index];
}

void IntersectedCell::update_last_segment_exit_point(Vector3d exit_point) {
  exit_points_[exit_points_.size()-1] = exit_point;
}

int IntersectedCell::num_segments() const{
  return entry_points_.size();
}

void IntersectedCell::add_new_segment(Vector3d entry_point, Vector3d exit_point,
                                      double radius, double skin_factor) {
  entry_points_.push_back(entry_point);
  exit_points_.push_back(exit_point);
  segment_radius_.push_back(radius);
  segment_skin_.push_back(skin_factor);
}

double IntersectedCell::cell_well_index_matrix() const {
  return well_index_matrix_;
}

void IntersectedCell::set_cell_well_index_matrix(double well_index) {
  well_index_matrix_ = well_index;
}

double IntersectedCell::cell_well_index_fracture() const {
  return well_index_fracture_;
}

void IntersectedCell::set_cell_well_index_fracture(double well_index) {
  well_index_fracture_ = well_index;
}

void IntersectedCell::set_segment_calculation_data(int segment_index,
                                                   string name,
                                                   double value) {
  // Check if this name already exists
  map<string, vector<double>>::iterator it = calculation_data_.find(name);

  if(it != calculation_data_.end()) {
    if (segment_index >= 0 && segment_index < calculation_data_[name].size()) {
      calculation_data_[name].at(segment_index) = value;
    }
    else if(segment_index == calculation_data_[name].size()) {
      calculation_data_[name].push_back(value);
    }
    else {
      runtime_error("This segment index is out of bounds.");
    }
  }
  else {
    calculation_data_[name].push_back(value);
  }
}

map<string, vector<double>>& IntersectedCell::get_calculation_data() {
  return calculation_data_;
}

void IntersectedCell::set_segment_calculation_data_3d(int segment_index,
                                                   string name,
                                                      Vector3d value3d) {
  // Check if this name already exists
  map<string, vector<Vector3d>>::iterator
      it = calculation_data_3d_.find(name);

  if(it != calculation_data_3d_.end()) {
    if (segment_index >= 0 && segment_index < calculation_data_3d_[name].size()) {
      calculation_data_3d_[name].at(segment_index) = value3d;
    }
    else if(segment_index == calculation_data_3d_[name].size()) {
      calculation_data_3d_[name].push_back(value3d);
    }
    else {
      runtime_error("This segment index is out of bounds.");
    }
  }
  else {
    calculation_data_3d_[name].push_back(value3d);
  }
}

map<string, vector<Vector3d>>& IntersectedCell::get_calculation_data_3d() {
  return calculation_data_3d_;
}

int IntersectedCell::GetIntersectedCellIndex(vector<IntersectedCell> &cells,
                                             Grid::Cell grdcell){
  if (cells.size() == 0) {
    cells.push_back(IntersectedCell(grdcell));
    return 0;
  }
  else {
    for(int cell_index = 0 ; cell_index < cells.size(); cell_index++) {
      if (cells.at(cell_index).global_index() == grdcell.global_index()) {
        return cell_index;
      }
    }

    cells.push_back(IntersectedCell(grdcell));
    return cells.size() - 1;
  }
}


}
}
