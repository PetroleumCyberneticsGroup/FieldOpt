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

#include "polar_azimuth.h"
namespace Optimization {
namespace Constraints {

PolarAzimuth::PolarAzimuth(Settings::Optimizer::Constraint settings,
                           Model::Properties::VariablePropertyContainer *variables) {
  min_azimuth_ = settings.min;
  max_azimuth_ = settings.max;

  for (auto var : variables->GetContinousVariables()->values()){
    if (var->propertyInfo().polar_prop == Model::Properties::Property::PolarProp::Azimuth
    && QString::compare(var->propertyInfo().parent_well_name, settings.well) == 0){
      affected_variable_ = var->id();
      break;
    }
  }
  if (affected_variable_.isNull()) throw std::runtime_error("Affected variable null in PolarAzimuth constraint.");
}

bool PolarAzimuth::CaseSatisfiesConstraint(Optimization::Case *c) {
  if (c->real_variables()[affected_variable_] <= max_azimuth_
    && c->real_variables()[affected_variable_] >= min_azimuth_){
    return true;
  } else {
    return false;
  }
}

void PolarAzimuth::SnapCaseToConstraints(Optimization::Case *c) {
  if (c->real_variables()[affected_variable_] >= max_azimuth_){
    c->set_real_variable_value(affected_variable_, max_azimuth_);
  } else if (c->real_variables()[affected_variable_] <= min_azimuth_) {
    c->set_real_variable_value(affected_variable_, min_azimuth_);
  }
}

bool PolarAzimuth::IsBoundConstraint() const {
  return true;
}
Eigen::VectorXd PolarAzimuth::GetLowerBounds(QList<QUuid> id_vector) const {
  Eigen::VectorXd lbounds(id_vector.size());
  lbounds.fill(0);
  lbounds[id_vector.indexOf(affected_variable_)] = min_azimuth_;
  return lbounds;
}
Eigen::VectorXd PolarAzimuth::GetUpperBounds(QList<QUuid> id_vector) const {
  Eigen::VectorXd ubounds(id_vector.size());
  ubounds.fill(0);
  ubounds[id_vector.indexOf(affected_variable_)] = max_azimuth_;
  return ubounds;
}
string PolarAzimuth::name() {
  return "PolarAzimuth";
}

}
}