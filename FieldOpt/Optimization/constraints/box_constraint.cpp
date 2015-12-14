/******************************************************************************
 *
 *
 *
 * Created: 03.12.2015 2015 by einar
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

#include "box_constraint.h"
#include "stdexcept"

namespace Optimization { namespace Constraints {

BoxConstraint::BoxConstraint(Utilities::Settings::Optimizer::Constraint settings, Model::Properties::VariablePropertyContainer *variables)
    : Constraint(settings, variables)
{
    switch (settings.type) {
    case ::Utilities::Settings::Optimizer::ConstraintType::BHP:
        initializeBhpConstraints(settings.min, settings.max);
        break;
    default:
        throw std::runtime_error("Constraint type not recognized.");
    }
}

bool BoxConstraint::CaseSatisfiesConstraint(Case *c)
{
    foreach (QUuid id, c->integer_variables().keys()) {
        if (integer_constraints_.contains(id)) {
            if (!(c->integer_variables()[id] >= integer_constraints_[id].first && c->integer_variables()[id] <= integer_constraints_[id].second))
                return false;
        }
    }
    foreach (QUuid id, c->real_variables().keys()) {
        if (real_constraints_.contains(id)) {
            if (!(c->real_variables()[id] >= real_constraints_[id].first && c->real_variables()[id] <= real_constraints_[id].second))
                return false;
        }
    }
    return true;
}

void BoxConstraint::SnapCaseToConstraints(Case *c)
{
    foreach (QUuid id, c->integer_variables().keys()) {
        if (integer_constraints_.contains(id)) {
            if (c->integer_variables()[id] < integer_constraints_[id].first)
                c->set_integer_variable_value(id, integer_constraints_[id].first);
            else if (c->integer_variables()[id] > integer_constraints_[id].second)
                c->set_integer_variable_value(id, integer_constraints_[id].second);
        }
    }
    foreach (QUuid id, c->real_variables().keys()) {
        if (real_constraints_.contains(id)) {
            if (c->real_variables()[id] < real_constraints_[id].first)
                c->set_real_variable_value(id, real_constraints_[id].first);
            else if (c->real_variables()[id] > real_constraints_[id].second)
                c->set_real_variable_value(id, real_constraints_[id].second);
        }
    }
}

void BoxConstraint::initializeBhpConstraints(double min, double max)
{
    integer_constraints_ = QHash<QUuid, QPair<double, double> >();
    real_constraints_ = QHash<QUuid, QPair<double, double> >();
    foreach (QUuid id, affected_real_variables_) {
        real_constraints_[id] = QPair<double,double>(min, max);
    }
}

}}
