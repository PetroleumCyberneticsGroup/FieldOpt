/******************************************************************************
 *
 * trajectory.h
 *
 * Created: 24.09.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "wellblock.h"
#include "wellspline.h"
#include "completions/completion.h"
#include "completions/perforation.h"
#include "Utilities/settings/model.h"
#include "Model/variables/variablecontainer.h"
#include "Model/variables/variablehandler.h"

#include <QList>

namespace Model {
namespace Wells {
namespace Wellbore {

/*!
 * \brief The Trajectory class describes the trajectory of the wellbore as a set of well blocks
 * in the reservoir grid.
 *
 * A trajectory may be described either as a set of well blocks directly, or as a spline which
 * a set of well blocks is calculated from. Either way it is always percieved as a set of blocks
 * from the outside.
 *
 * \todo Implement support for SplinePoint definition. Decide how to add completions to it. SplinePoints should provide a WellBlock list.
 *
 * \todo Initialize ICDs.
 *
 * \todo Handle variable trajectories.
 */
class Trajectory
{
public:
    Trajectory(::Utilities::Settings::Model::Well well_settings,
               Variables::VariableContainer *variable_container,
               Variables::VariableHandler *variable_handler);
    WellBlock *GetWellBlock(int i, int j, int k); //!< Get the well block at index (i,j,k).
    QList<WellBlock *> *GetWellBlocks(); //!< Get a list containing all well blocks.

private:
    QList<WellBlock *> *well_blocks_;

    void initializeWellBlocks(Utilities::Settings::Model::Well well,
                              Variables::VariableContainer *variable_container,
                              Variables::VariableHandler *variable_handler);

    Completions::Completion *getCompletion(QList<Utilities::Settings::Model::Completion> completions,
                                           Utilities::Settings::Model::IntegerCoordinate block,
                                           Variables::VariableContainer *variable_container,
                                           Variables::VariableHandler *variable_handler);
};

}
}
}

#endif // TRAJECTORY_H
