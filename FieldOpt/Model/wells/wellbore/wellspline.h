/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#ifndef WELLSPLINE_H
#define WELLSPLINE_H

#include "trajectory.h"
#include "Reservoir/grid/eclgrid.h"
#include "Model/wells/wellbore/wellblock.h"
#include "FieldOpt-WellIndexCalculator/wellindexcalculator.h"
#include <QList>

namespace Model {
namespace Wells {
namespace Wellbore {

using namespace Model::Properties;

/*!
 * \brief The WellSpline class Generates the well blocks making up the trajectory from a set of spline points.
 * It uses the WellIndexCalculation library to do this.
 */
class WellSpline
{
 public:
  WellSpline(::Settings::Model::Well well_settings,
             Properties::VariablePropertyContainer *variable_container,
             Reservoir::Grid::Grid *grid);

  /*!
   * \brief GetWellBlocks Get the set of well blocks with proper WI's defined by the spline.
   * \return
   */
  QList<WellBlock *> *GetWellBlocks();
  int GetTimeSpentInWIC() const { return seconds_spent_in_compute_wellblocks_; }

  struct SplinePoint {
    ContinousProperty *x;
    ContinousProperty *y;
    ContinousProperty *z;
    Eigen::Vector3d ToEigenVector() const;
  };

 private:
  Reservoir::Grid::Grid *grid_;
  Settings::Model::Well well_settings_;
  int seconds_spent_in_compute_wellblocks_; //!< Number of seconds spent in the ComputeWellBlocks() method.

  QList<SplinePoint *> spline_points_;

  /*!
   * \brief getWellBlock Convert the BlockData returned by the WIC to a WellBlock with a Perforation.
   * \note The IJK indexes are incremented by on to account for the zero-inclusive indices used by
   * the ERT library. This is necessary because ECL and ADGPRS both use zero-exclusive indices.
   * \param block_data
   * \return
   */
  WellBlock *getWellBlock(Reservoir::WellIndexCalculation::IntersectedCell block_data);
};

}
}
}

#endif // WELLSPLINE_H
