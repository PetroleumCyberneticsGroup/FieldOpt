/******************************************************************************
   Created by einar on 4/4/19.
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#ifndef FIELDOPT_K_LOCKED_POLAR_SPLINE_H
#define FIELDOPT_K_LOCKED_POLAR_SPLINE_H

#include "wellspline.h"

namespace Model {
namespace Wells {
namespace Wellbore {

class KLockedPolarSpline : public WellSpline {

 public:
  KLockedPolarSpline(::Settings::Model::Well well_settings,
  Properties::VariablePropertyContainer *variable_container,
      Reservoir::Grid::Grid *grid,
  Reservoir::WellIndexCalculation::wicalc_rixx *wic
  );

  /*!
   * @brief This function activates the WellIndexCalculator (WIC) and returns the blocks between the two
   * Spline Points as defined by the midpoint, azimuth, elevation and length.
   * @return
   */
  virtual QList<WellBlock *> *GetWellBlocks();

 private:
  int k_;                            //!< The vertical layer to which this well is locked.
  Reservoir::Grid::Cell heel_cell_;  //!< The cell the heel of the well is currently placed in.
  Reservoir::Grid::Cell toe_cell_;   //!< The cell the toe of the well is currently placed in.
  Properties::ContinousProperty *x_; //!< The x-placement of the well heel.
  Properties::ContinousProperty *y_; //!< The y-placement of the well heel.
  Properties::ContinousProperty *azimuth_; //!< The azimuth rotation in the x-y plane, with 0 degrees being parallel to the x axis
  Properties::ContinousProperty *length_; //!< The length of the well in meters, length/2 is used as the radius

  /*!
   * @brief Update heel_cell_ and spline_points_ according to the current x_, y_ and k_ member values.
   * @return
   */
  void updateEndpointCells();

  /*!
   * @brief Get the four cells surrounding the current_cell.
   * @param avoid_idx Global index of cell to avoid (e.g. previous cell to avoid cycles).
   * @return Vector of four cells, ordered as [-i, +i, -j, +j] (given that all exist).
   */
  std::vector<Reservoir::Grid::Cell> getAdjacentCells(const int avoid_idx, Reservoir::Grid::Cell current_cell) const;

  std::pair<Reservoir::Grid::Cell, Eigen::Vector3d> findNext(Reservoir::Grid::Cell previous_cell, Reservoir::Grid::Cell current_cell, Eigen::Vector3d current_point) const;

 protected:
  vector<Vector3d> getPoints() const override;
};

}
}
}

#endif //FIELDOPT_K_LOCKED_POLAR_SPLINE_H
