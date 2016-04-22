/******************************************************************************
 *
 *
 *
 * Created: 04.12.2015 2015 by einar
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

#ifndef CONSTRAINTHANDLER_H
#define CONSTRAINTHANDLER_H
#include "constraint.h"
#include "box_constraint.h"
#include "well_spline_length.h"
#include "interwell_distance.h"
#include "combined_spline_length_interwell_distance.h"
#include "Optimization/case.h"
#include "Model/properties/variable_property_container.h"
#include "Utilities/settings/optimizer.h"
#include <QList>

namespace Optimization { namespace Constraints {

/*!
 * \brief The ConstraintHandler class facilitiates the initialization and usage of multiple constraints.
 */
class ConstraintHandler
{
public:
    ConstraintHandler(QList<Utilities::Settings::Optimizer::Constraint> *constraints, Model::Properties::VariablePropertyContainer *variables);
    bool CaseSatisfiesConstraints(Case *c); //!< Check if a Case satisfies _all_ constraints.
    void SnapCaseToConstraints(Case *c); //!< Snap all variables to _all_ constraints.

    QList<Constraint *> constraints() const { return constraints_; }

private:
    QList<Constraint *> constraints_;
};

}}

#endif // CONSTRAINTHANDLER_H
