/******************************************************************************
 *
 * wellspline.h
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

#ifndef WELLSPLINE_H
#define WELLSPLINE_H

#include "trajectory.h"
#include "Model/reservoir/reservoir.h"
#include "Model/wells/wellbore/wellblock.h"
#include "WellIndexCalculator/wellindexcalculator.h"
#include <QList>

namespace Model {
namespace Wells {
namespace Wellbore {

/*!
 * \brief The WellSpline class Generates the well blocks making up the trajectory from a set of spline points.
 * It uses the WellIndexCalculator library to do this.
 */
class WellSpline
{
public:
    WellSpline(::Utilities::Settings::Model::Well well_settings,
               Properties::VariablePropertyContainer *variable_container,
               Properties::VariablePropertyHandler *variable_handler,
               ::Model::Reservoir::Reservoir *reservoir);

    /*!
     * \brief GetWellBlocks Get the set of well blocks with proper WI's defined by the spline.
     * \return
     */
    QList<WellBlock *> *GetWellBlocks();

private:
    Model::Reservoir::Grid::Grid *grid_;
   Utilities::Settings::Model::Well well_settings_;

   Model::Properties::ContinousProperty *heel_x_;
   Model::Properties::ContinousProperty *heel_y_;
   Model::Properties::ContinousProperty *heel_z_;
   Model::Properties::ContinousProperty *toe_x_;
   Model::Properties::ContinousProperty *toe_y_;
   Model::Properties::ContinousProperty *toe_z_;

   /*!
    * \brief getWellBlock Convert the BlockData returned by the WIC to a WellBlock with a Perforation.
    * \note The IJK indexes are incremented by on to account for the zero-inclusive indices used by
    * the ERT library. This is necessary because ECL and ADGPRS both use zero-exclusive indices.
    * \param block_data
    * \return
    */
   WellBlock *getWellBlock(WellIndexCalculator::WellIndexCalculator::BlockData block_data);
   int block_id_; //!< An ID that will be passed to the well block.
};

}
}
}

#endif // WELLSPLINE_H
