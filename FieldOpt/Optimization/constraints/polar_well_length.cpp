/******************************************************************************
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>,
   Brage Strand Kristoffersen <brage_sk@hotmail.com>

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

#include "polar_well_length.h"
#include "constraint.h"

namespace Optimization {
namespace Constraints {
PolarWellLength::PolarWellLength(Settings::Optimizer::Constraint settings,
                                   Model::Properties::VariablePropertyContainer *variables) {
  minimum_length_ = settings.min_length;
  maximum_length_ = settings.max_length;

  for (auto var : variables->GetContinousVariables()->values()) {
    if (var->propertyInfo().parent_well_name == settings.well
    && var->propertyInfo().polar_prop == Model::Properties::Property::PolarProp::Length) {
      affected_variable_ = var->id();
      break;
    }
  }
}

bool PolarWellLength::CaseSatisfiesConstraint(Case *c) {
  if (c->real_variables()[affected_variable_] <= maximum_length_
  && c->real_variables()[affected_variable_] >= minimum_length_){
    return true;
  } else {
    return false;
  }
}
void PolarWellLength::SnapCaseToConstraints(Case *c) {
  if (c->real_variables()[affected_variable_] > maximum_length_){
    c->set_real_variable_value(affected_variable_, maximum_length_);
  } else if (c-> real_variables()[affected_variable_] < minimum_length_){
    c->set_real_variable_value(affected_variable_, minimum_length_);
  }
}
bool PolarWellLength::IsBoundConstraint() const {
  return true;
}
Eigen::VectorXd PolarWellLength::GetLowerBounds(QList<QUuid> id_vector) const {
  Eigen::VectorXd lbounds(id_vector.size());
  lbounds.fill(0);
  lbounds[id_vector.indexOf(affected_variable_)] = minimum_length_;
  return lbounds;
}
Eigen::VectorXd PolarWellLength::GetUpperBounds(QList<QUuid> id_vector) const {
  Eigen::VectorXd ubounds(id_vector.size());
  ubounds.fill(0);
  ubounds[id_vector.indexOf(affected_variable_)] = maximum_length_;
  return ubounds;
}
string PolarWellLength::name() {
  return "PolarWellLength";
}
}
}