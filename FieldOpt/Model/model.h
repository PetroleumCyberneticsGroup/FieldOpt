/******************************************************************************
 *
 * model.h
 *
 * Created: 22.09.2015 2015 by einar
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

#ifndef MODEL_H
#define MODEL_H

#include <QString>
#include <QList>

#include "reservoir/reservoir.h"
#include "properties/variable_property_container.h"
#include "properties/variable_property_handler.h"
#include "wells/well.h"
#include "Utilities/settings/model.h"
#include "Optimization/case.h"
#include "Model/wells/wellbore/wellblock.h"

namespace Model {

/*!
 * \brief The Model class represents the reservoir model as a whole, including wells and
 * any related variables, and the reservoir grid.
 */
class Model
{
public:
    Model(::Utilities::Settings::Model settings);

    /*!
     * \brief reservoir Get the reservoir (i.e. grid).
     */
    Reservoir::Reservoir *reservoir() const { return reservoir_; }

    /*!
     * \brief variables Get the set of variable properties of all types.
     */
    Properties::VariablePropertyContainer *variables() const { return variable_container_; }

    /*!
     * \brief wells Get a list of all the wells in the model.
     */
    QList<Wells::Well *> *wells() const { return wells_; }

    /*!
     * \brief ApplyCase Applies the variable values from a case to the variables in the model.
     * \param c Case to apply the variable values of.
     */
    void ApplyCase(Optimization::Case *c);

private:
    Reservoir::Reservoir *reservoir_;
    Properties::VariablePropertyContainer *variable_container_;
    Properties::VariablePropertyHandler *variable_handler_;
    QList<Wells::Well *> *wells_;

    void verify(); //!< Verify the model. Throws an exception if it is not.
    void verifyWells();
    void verifyWellTrajectory(Wells::Well *w);
    void verifyWellBlock(Wells::Wellbore::WellBlock *wb);
};

}

#endif // MODEL_H
