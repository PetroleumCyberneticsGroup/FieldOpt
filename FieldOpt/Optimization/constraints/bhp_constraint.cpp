/******************************************************************************
   Copyright (C) 2016-2017 Einar J.M. Baumann <einar.baumann@gmail.com>
   Modified 2017/08/22 by Einar Einar J.M. Baumann

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

#include <iostream>
#include "bhp_constraint.h"

namespace Optimization {
namespace Constraints {

BhpConstraint::BhpConstraint(Settings::Optimizer::Constraint settings, Model::Properties::VariablePropertyContainer *variables)
{


    assert(settings.wells.size() > 0);
    assert(settings.min < settings.max);

    min_ = settings.min;
    max_ = settings.max;
    affected_well_names_ = settings.wells;
    penalty_weight_ = settings.penalty_weight;
    for (auto wname : affected_well_names_) {
        affected_real_variables_.append(variables->GetWellBHPVariables(wname));
    }
}

bool BhpConstraint::CaseSatisfiesConstraint(Case *c)
{
    for (auto var : affected_real_variables_) {
        double case_value = c->real_variables()[var->id()];
        if (case_value > max_ || case_value < min_)
            return false;
    }
    return true;
}
bool BhpConstraint::IsBoundConstraint() const {
    return true;
}
Eigen::VectorXd BhpConstraint::GetLowerBounds(QList<QUuid> id_vector) const {
    Eigen::VectorXd lbounds(id_vector.size());
    lbounds.fill(0);
    for (auto var : affected_real_variables_) {
        lbounds[id_vector.indexOf(var->id())] = min_;
    }
    return lbounds;
}
Eigen::VectorXd BhpConstraint::GetUpperBounds(QList<QUuid> id_vector) const {
    Eigen::VectorXd ubounds(id_vector.size());
    ubounds.fill(0);
    for (auto var : affected_real_variables_) {
        ubounds[id_vector.indexOf(var->id())] = max_;
    }
    return ubounds;
}

void BhpConstraint::SnapCaseToConstraints(Case *c)
{
    for (auto var : affected_real_variables_) {
        if (c->real_variables()[var->id()] > max_)
            c->set_real_variable_value(var->id(), max_);
        else if (c->real_variables()[var->id()] < min_)
            c->set_real_variable_value(var->id(), min_);
    }
}

}
}
