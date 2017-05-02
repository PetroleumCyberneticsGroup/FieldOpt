/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "wellblock.h"
#include "wellspline.h"
#include "completions/completion.h"
#include "completions/perforation.h"
#include "Reservoir/grid/eclgrid.h"
#include "Settings/model.h"
#include "Model/properties/variable_property_container.h"
#include "Model/properties/property.h"

#include <QList>

namespace Model {
namespace Wells {
namespace Wellbore {

class WellSpline;

/*!
 * \brief The Trajectory class describes the trajectory of the wellbore as a set of well blocks
 * in the reservoir grid.
 *
 * A trajectory may be described either as a set of well blocks directly, or as a spline which
 * a set of well blocks is calculated from. Either way it is always percieved as a set of blocks
 * from the outside.
 *
 * \todo Initialize ICDs.
 */
class Trajectory
{
 public:
  Trajectory(::Settings::Model::Well well_settings,
             Properties::VariablePropertyContainer *variable_container,
             Reservoir::Grid::Grid *grid);
  WellBlock *GetWellBlock(int i, int j, int k); //!< Get the well block at index (i,j,k).
  QList<WellBlock *> *GetWellBlocks(); //!< Get a list containing all well blocks.
  void UpdateWellBlocks(); //!< Update the well blocks, in particular the ones defined by a spline.
  int GetTimeSpentInWic() const;

 private:
  QList<WellBlock *> *well_blocks_;

  WellSpline *well_spline_; //!< Used to defined trajectories with a spline. When used, this generates the well blocks.

  void initializeWellBlocks(Settings::Model::Well well,
                            Properties::VariablePropertyContainer *variable_container);

  void calculateDirectionOfPenetration(); // Calculate direction of penetration for all well blocks

};

}
}
}

#endif // TRAJECTORY_H
