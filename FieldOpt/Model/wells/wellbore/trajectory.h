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
#include "pseudo_cont_vert.h"

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
  Settings::Model::WellDefinitionType GetDefinitionType();
  double GetLength() const; //!< Get the length of the wellbore (measured depth from the heel to the toe)
  WellBlock *GetWellBlockByMd(double md) const; //!< Get the wellblock surrounding the given MD.
  double GetEntryMd(const WellBlock *wb) const; //!< Get the measured depth for the entry point to the block.
  double GetExitMd(const WellBlock *wb) const; //!< Get the measured depth for the exit point from the block.

 private:
  Settings::Model::WellDefinitionType definition_type_;
  QList<WellBlock *> *well_blocks_;

  WellSpline *well_spline_; //!< Used to defined trajectories with a spline. When used, this generates the well blocks.
  PseudoContVert *pseudo_cont_vert_; //!< A pseudo-continuous vertical well.

  void initializeWellBlocks(Settings::Model::Well well,
                            Properties::VariablePropertyContainer *variable_container);

  void calculateDirectionOfPenetration(); // Calculate direction of penetration for all well blocks

  /*!
   * Convert the list of well blocks in the well settings object to a well spline, with the
   * number of spline points specified in the settings object (defaulting to 2).
   *
   * The spline points will be selected naively. E.g. for three spline points:
   * the heel point will be the center of the first well block in the list;
   * the center point will be the center of the middle well block in the list ( list[size/2] )
   * and the toe point the center of the last well block;
   */
  void convertWellBlocksToWellSpline(Settings::Model::Well &well_settings, Reservoir::Grid::Grid *grid);

  bool is_2d_; //!< Indicates if the well should only be able to vary in the x-y plane (z variables will not be created).

  void printWellBlocks();

};

}
}
}

#endif // TRAJECTORY_H
