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

#include "constraint_handler.h"

namespace Optimization { namespace Constraints {

ConstraintHandler::ConstraintHandler(QList<Utilities::Settings::Optimizer::Constraint> *constraints, Model::Properties::VariablePropertyContainer *variables)
{
    foreach (Utilities::Settings::Optimizer::Constraint constraint, *constraints) {
        switch (constraint.type) {
        case Utilities::Settings::Optimizer::ConstraintType::BHP:
            constraints_.append(new BoxConstraint(constraint, variables));
            break;
        case Utilities::Settings::Optimizer::ConstraintType::WellSplineLength:
            constraints_.append(new WellSplineLength(constraint, variables));
            break;
        case Utilities::Settings::Optimizer::ConstraintType::WellSplineInterwellDistance:
            constraints_.append(new InterwellDistance(constraint, variables));
            break;
        case Utilities::Settings::Optimizer::ConstraintType::CombinedWellSplineLengthInterwellDistance:
            constraints_.append(new CombinedSplineLengthInterwellDistance(constraint, variables));
            break;
        default:
            break;
        }
    }
}

bool ConstraintHandler::CaseSatisfiesConstraints(Case *c)
{
    foreach (Constraint *constraint, constraints_) {
        if (!constraint->CaseSatisfiesConstraint(c))
            return false;
    }
    return true;
}

void ConstraintHandler::SnapCaseToConstraints(Case *c)
{
    foreach (Constraint *constraint, constraints_) {
        constraint->SnapCaseToConstraints(c);
    }
}



}}
