/******************************************************************************
   Copyright (C) 2015-2016 Hilmar M. Magnusson <hilmarmag@gmail.com>
   Modified by Einar J.M. Baumann (2016) <einar.baumann@gmail.com>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>

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

#ifndef FIELDOPT_INTERSECTEDCELL_H
#define FIELDOPT_INTERSECTEDCELL_H

#include "Reservoir/grid/cell.h"
#include <map>
#include <vector>

namespace Reservoir {
namespace WellIndexCalculation {
using namespace Eigen;
using namespace std;

/*!
 * \brief The IntersectedCell struct holds
 * information about an intersected cell.
 */
class IntersectedCell : public Grid::Cell {
 public:
  IntersectedCell() {}
  IntersectedCell(const Grid::Cell &cell);

  /*!
   * \brief The cell x axis
   */
  Vector3d xvec() const;
  /*!
   * \brief The cell y axis
   */
  Vector3d yvec() const;
  /*!
   * \brief The cell z axis
   */
  Vector3d zvec() const;

  void add_new_segment(Vector3d entry_point, Vector3d exit_point,
                       double segment_radius, double segment_skin);
  int num_segments() const;

  Vector3d get_segment_entry_point(int segment_index) const;
  Vector3d get_segment_exit_point(int segment_index) const;
  double get_segment_radius(int segment_index) const;
  double get_segment_skin(int segment_index) const;

  void update_last_segment_exit_point(Vector3d exit_point);

  double cell_well_index_matrix() const;
  void set_cell_well_index_matrix(double well_index);

  double cell_well_index_fracture() const;
  void set_cell_well_index_fracture(double well_index);

  void set_segment_calculation_data(int segment_index,
                                    string name,
                                    double value);

  void set_segment_calculation_data_3d(int segment_index,
                                    string name,
                                       Vector3d value3d);

  map<string, vector<double>>& get_calculation_data();
  map<string, vector<Vector3d>>& get_calculation_data_3d();

  /*!
   * @brief Get the index of an intersected cell. If it is not found in
   * the already intersected cell set, then the cell is added to this
   * set before the new index is returned.
   * @param cells The cell to find the index for.
   * @param grdcell The set of intersected cells already found.
   * @return The index of current grdcell in the intersected cells set.
   */
  static int GetIntersectedCellIndex(vector<IntersectedCell> &cells,
                                     Grid::Cell grdcell);

 private:
  // intersecting well segment definition
  vector<Vector3d> entry_points_;
  vector<Vector3d> exit_points_;
  vector<double> segment_radius_;
  vector<double> segment_skin_;

  int i_, j_, k_;

  // per segment well index calculation data
  map<string, vector<double>> calculation_data_;
  map<string, vector<Vector3d>> calculation_data_3d_;

  // well indices
  double well_index_matrix_;
  double well_index_fracture_;
};
}
}

#endif //FIELDOPT_INTERSECTEDCELL_H
