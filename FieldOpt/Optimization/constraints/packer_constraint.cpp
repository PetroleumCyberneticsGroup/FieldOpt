/******************************************************************************
   Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include "packer_constraint.h"

namespace Optimization {
namespace Constraints {

using namespace Model::Properties;

PackerConstraint::PackerConstraint(Settings::Optimizer::Constraint settings,
                                                              Model::Properties::VariablePropertyContainer *variables) {
    min_ = settings.min_md;
    max_ = settings.max_md;
    for (ContinousProperty *var : variables->GetContinousVariables()->values()) {
        if (var->propertyInfo().prop_type == Property::PropertyType::Packer
            && QString::compare(var->propertyInfo().parent_well_name, settings.well) == 0) {
            affected_variables_.push_back(var->id());
        }
    }
}
string PackerConstraint::name() {
    return "PackerConstraint";
}

bool PackerConstraint::CaseSatisfiesConstraint(Optimization::Case *c) {
    for (auto id : affected_variables_) {
        if (c->real_variables()[id] > max_ || c->real_variables()[id] < min_) {
            return false;
        }
    }
    return true;
}
void PackerConstraint::SnapCaseToConstraints(Optimization::Case *c) {
    for (auto id : affected_variables_) {
        if (c->real_variables()[id] > max_) {
            c->set_real_variable_value(id, max_);
            if (verbosity_level_ > 1) {
                std::cout << "IN OPTIMIZER, PackerConstriant: Snapped value to upper bound." << std::endl;
            }
        }
        else if (c->real_variables()[id] < min_) {
            c->set_real_variable_value(id, min_);
            if (verbosity_level_ > 1) {
                std::cout << "IN OPTIMIZER, PackerConstriant: Snapped value to lower bound." << std::endl;
            }
        }
    }
}
bool PackerConstraint::IsBoundConstraint() const {
    return true;
}
Eigen::VectorXd PackerConstraint::GetLowerBounds(QList<QUuid> id_vector) const {
    Eigen::VectorXd lbounds(id_vector.size());
    lbounds.fill(0);
    for (auto id : affected_variables_) {
        lbounds[id_vector.indexOf(id)] = min_;
    }
    return lbounds;
}
Eigen::VectorXd PackerConstraint::GetUpperBounds(QList<QUuid> id_vector) const {
    Eigen::VectorXd ubounds(id_vector.size());
    ubounds.fill(0);
    for (auto id : affected_variables_) {
        ubounds[id_vector.indexOf(id)] = max_;
    }
    return ubounds;
}

}
}
