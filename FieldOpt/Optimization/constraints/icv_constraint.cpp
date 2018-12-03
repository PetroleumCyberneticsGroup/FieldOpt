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

#include <Utilities/printer.hpp>
#include <Utilities/verbosity.h>
#include "icv_constraint.h"
#include "Utilities/stringhelpers.hpp"

namespace Optimization {
namespace Constraints {

using namespace Model::Properties;

ICVConstraint::ICVConstraint(Settings::Optimizer::Constraint settings,
                             Model::Properties::VariablePropertyContainer *variables) {
    min_ = settings.min;
    max_ = settings.max;
    Printer::ext_info("Adding ICV constraint with [min, max] = [" + Printer::num2str(min_)
                       + ", " + Printer::num2str(max_) + "] for well " + settings.well.toStdString(),
                       "Optimizer", "ICVConstraint");
    for (ContinousProperty *var : variables->GetContinousVariables()->values()) {
        if (var->propertyInfo().prop_type == Property::PropertyType::ICD
            && QString::compare(var->propertyInfo().parent_well_name, settings.well) == 0) {
            affected_variables_.push_back(var->id());
            Printer::info("Added ICV constraint for variable " + var->name().toStdString());
        }
    }
}

bool ICVConstraint::CaseSatisfiesConstraint(Optimization::Case *c) {
    for (auto id : affected_variables_) {
        if (c->real_variables()[id] > max_ || c->real_variables()[id] < min_) {
            return false;
        }
    }
    return true;
}
void ICVConstraint::SnapCaseToConstraints(Optimization::Case *c) {
    if (VERB_OPT >= 1) {
        Printer::ext_info("Checking case with vars { " + eigenvec_to_str(c->GetRealVarVector()) + " } "
            + "against constraint [" + Printer::num2str(min_) + ", " + Printer::num2str(max_) + "]",
            "Optimization", "ICVConstraint"
            );
    }
    for (auto id : affected_variables_) {
        if (c->real_variables()[id] > max_) {
            c->set_real_variable_value(id, max_);
            if (VERB_OPT >= 1) { Printer::ext_info("Snapped value to upper bound.", "Optimization", "ICVConstraint"); }
        }
        else if (c->real_variables()[id] < min_) {
            c->set_real_variable_value(id, min_);
            if (VERB_OPT >= 1) { Printer::ext_info("Snapped value to lower bound.", "Optimization", "ICVConstraint"); }
        }
    }
}
bool ICVConstraint::IsBoundConstraint() const {
    true;
}
Eigen::VectorXd ICVConstraint::GetLowerBounds(QList<QUuid> id_vector) const {
    Eigen::VectorXd lbounds(id_vector.size());
    lbounds.fill(0);
    for (auto id : affected_variables_) {
        lbounds[id_vector.indexOf(id)] = min_;
    }
    return lbounds;
}
Eigen::VectorXd ICVConstraint::GetUpperBounds(QList<QUuid> id_vector) const {
    Eigen::VectorXd ubounds(id_vector.size());
    ubounds.fill(0);
    for (auto id : affected_variables_) {
        ubounds[id_vector.indexOf(id)] = max_;
    }
    return ubounds;
}
string ICVConstraint::name() {
    return "ICVConstraint";
}

}
}
