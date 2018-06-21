/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include "wcntrl_rate.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
RateConstraint::RateConstraint(
    Settings::Optimizer::Constraint settings,
    Model::Properties::VariablePropertyContainer *variables) {

  // -------------------------------------------------------
  affected_well_names_ = settings.wells;
  min_ = settings.min;
  max_ = settings.max;
  penalty_weight_ = settings.penalty_weight;

  // -------------------------------------------------------
  for (auto wname : affected_well_names_) {
    affected_real_variables_.append(
        variables->GetWellRateVariables(wname));
  }

}

// =========================================================
bool RateConstraint::CaseSatisfiesConstraint(Case *c) {

  // -------------------------------------------------------
  for (auto var : affected_real_variables_) {
    double case_value = c->real_variables()[var->id()];
    if (case_value > max_ || case_value < min_) {
      return false;
    }
  }
  return true;
}

// =========================================================
void RateConstraint::SnapCaseToConstraints(Case *c) {

  // -------------------------------------------------------
  for (auto var : affected_real_variables_) {
    if (c->real_variables()[var->id()] > max_) {
      c->set_real_variable_value(var->id(), max_);

    } else if (c->real_variables()[var->id()] < min_) {
      c->set_real_variable_value(var->id(), min_);
    }
  }
}

}
}
