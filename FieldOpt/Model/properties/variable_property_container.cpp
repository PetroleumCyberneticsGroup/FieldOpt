/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version
 3 of the License, or (at your option) any later version.

 FieldOpt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include "variable_property_container.h"
#include <QStringList>
#include <iostream>

// ---------------------------------------------------------
namespace Model {
namespace Properties {

// ---------------------------------------------------------
VariablePropertyContainer::VariablePropertyContainer() {

  binary_variables_ = new QHash<QUuid, BinaryProperty *>();
  discrete_variables_ = new QHash<QUuid, DiscreteProperty *>();
  continous_variables_ = new QHash<QUuid, ContinousProperty *>();
}

// ---------------------------------------------------------
void VariablePropertyContainer::
AddVariable(BinaryProperty *var) {

  var->SetVariable();
  binary_variables_->insert(var->id(), var);
}

// ---------------------------------------------------------
void VariablePropertyContainer::
AddVariable(DiscreteProperty *var) {

  var->SetVariable();
  discrete_variables_->insert(var->id(), var);
}

// ---------------------------------------------------------
void VariablePropertyContainer::
AddVariable(ContinousProperty *var) {

  var->SetVariable();
  continous_variables_->insert(var->id(), var);
}

// ---------------------------------------------------------
BinaryProperty *VariablePropertyContainer::
GetBinaryVariable(QUuid id) const {

  if (!binary_variables_->contains(id)) {
    throw VariableIdDoesNotExistException(
        "Binary variable not found.");
  } return binary_variables_->value(id);
}

// ---------------------------------------------------------
DiscreteProperty *VariablePropertyContainer::
GetDiscreteVariable(QUuid id) const {

  if (!discrete_variables_->contains(id)) {
    throw VariableIdDoesNotExistException(
        "Discrete variable not found.");
  } return discrete_variables_->value(id);
}

// ---------------------------------------------------------
ContinousProperty *VariablePropertyContainer::
GetContinousVariable(QUuid id) const {

  if (!continous_variables_->contains(id)) {
    throw VariableIdDoesNotExistException(
        "Continous variable not found.");
  } return continous_variables_->value(id);
}

// ---------------------------------------------------------
void VariablePropertyContainer::
SetBinaryVariableValue(QUuid id, bool val) {

  if (!binary_variables_->contains(id)) {
    throw VariableIdDoesNotExistException(
        "Binary variable not found.");
  } else binary_variables_->value(id)->setValue(val);
}

// ---------------------------------------------------------
void VariablePropertyContainer::
SetDiscreteVariableValue(QUuid id, int val) {

  if (!discrete_variables_->contains(id))
    throw VariableIdDoesNotExistException(
        "Integer variable not found.");
  else discrete_variables_->value(id)->setValue(val);
}

// =========================================================
void VariablePropertyContainer::
SetContinousVariableValue(QUuid id,
                          double val) {

  if (!continous_variables_->contains(id)) {
    throw VariableIdDoesNotExistException(
        "Continous variable not found.");
  } else continous_variables_->value(id)->setValue(val);
}

// ---------------------------------------------------------
QHash<QUuid, bool>
VariablePropertyContainer::GetBinaryVariableValues() const {

  QHash<QUuid, bool> binary_values = QHash<QUuid, bool>();
  for (QUuid key : binary_variables_->keys())
    binary_values[key] = binary_variables_->value(key)->value();
  return binary_values;
}

// ---------------------------------------------------------
QHash<QUuid, int>
VariablePropertyContainer::GetDiscreteVariableValues() const {

  QHash<QUuid, int> discrete_values = QHash<QUuid, int>();
  for (QUuid key : discrete_variables_->keys()) {
    discrete_values[key] = discrete_variables_->value(key)->value();
  }
  return discrete_values;
}

// ---------------------------------------------------------
QHash<QUuid, double>
VariablePropertyContainer::GetContinousVariableValues() const {

  QHash<QUuid, double> continous_values = QHash<QUuid, double>();
  for (QUuid key : continous_variables_->keys()) {
    continous_values[key] = continous_variables_->value(key)->value();
  }
  return continous_values;
}

// ---------------------------------------------------------
void VariablePropertyContainer::CheckVariableNameUniqueness() {

  QList<QString> names = QList<QString>();
  // -------------------------------------------------------
  for (auto var : discrete_variables_->values()) {
    if (var->name().size() > 0) {
      if (names.contains(var->name()))
        throw std::runtime_error(
            "Encountered non-unique variable name: "
                + var->name().toStdString());
      else
        names.append(var->name());
    }
  }

  // -------------------------------------------------------
  for (auto var : continous_variables_->values()) {
    if (var->name().size() > 0) {
      if (names.contains(var->name()))
        throw std::runtime_error(
            "Encountered non-unique variable name: "
                + var->name().toStdString());
      else
        names.append(var->name());
    }
  }

  // -------------------------------------------------------
  for (auto var : binary_variables_->values()) {
    if (var->name().size() > 0) {
      if (names.contains(var->name()))
        throw std::runtime_error(
            "Encountered non-unique variable name: "
                + var->name().toStdString());
      else
        names.append(var->name());
    }
  }
}

// ---------------------------------------------------------
QList<ContinousProperty *>
VariablePropertyContainer::GetWellSplineVariables(
    const QString well_name) const {

  QList<ContinousProperty *> spline_vars;
  // -------------------------------------------------------
  for (auto var : continous_variables_->values()) {
    if (var->propertyInfo().prop_type == Property::PropertyType::SplinePoint &&
        QString::compare(well_name, var->propertyInfo().parent_well_name) == 0) {
      spline_vars.append(var);
    }
  }
  return spline_vars;
}

// ---------------------------------------------------------
QHash<QUuid, double>
VariablePropertyContainer::GetWellSplineVariables() const {

  QHash<QUuid, double> spline_vars = QHash<QUuid, double>();
  // -------------------------------------------------------
  for (QUuid key : continous_variables_->keys()) {
    if (continous_variables_->value(key)->propertyInfo().prop_type
        == Property::PropertyType::SplinePoint) {
      spline_vars[key] = continous_variables_->value(key)->value();
    }
  }

  return spline_vars;
}

// ---------------------------------------------------------
QList<ContinousProperty *>
VariablePropertyContainer::GetPseudoContVertVariables(
    const QString well_name) const {

  QList<ContinousProperty *> pcv_vars;
  // -------------------------------------------------------
  for (auto var : continous_variables_->values()) {
    if (var->propertyInfo().prop_type == Property::PropertyType::PseudoContVert &&
        QString::compare(well_name, var->propertyInfo().parent_well_name) == 0) {
      pcv_vars.append(var);
    }
  }

  // -------------------------------------------------------
  if (pcv_vars.size() != 2) {
    std::cerr << "Error: Incorrect number ("
              << pcv_vars.size() << ") found for well "
              << well_name.toStdString() << std::endl;
    throw std::runtime_error(
        "Incorrect number of pseudocontinuous variables found.");
  }

  // -------------------------------------------------------
  if (pcv_vars[0]->propertyInfo().coord != Property::Coordinate::x)
    pcv_vars.swap(0, 1);
  return pcv_vars;
}

// ---------------------------------------------------------
QList<ContinousProperty *>
VariablePropertyContainer::GetWellControlVariables() const {

  QList<ContinousProperty *> well_controls;
  // -------------------------------------------------------
  for (auto var : continous_variables_->values()) {
    if (var->propertyInfo().prop_type == Property::PropertyType::BHP||
        var->propertyInfo().prop_type == Property::PropertyType::Rate) {
      well_controls.append(var);
    }
  }

  // -------------------------------------------------------
  std::sort(well_controls.begin(), well_controls.end(),
            [](ContinousProperty *p1, ContinousProperty *p2) {
              int time_1 = p1->propertyInfo().index;
              int time_2 = p2->propertyInfo().index;
              return time_1 < time_2;
            });
  return well_controls;
}

// ---------------------------------------------------------
QList<ContinousProperty *>
VariablePropertyContainer::GetWellControlVariables(
    const QString well_name) const {

  QList<ContinousProperty *> well_controls;
  for (auto var : GetWellControlVariables()) {
    if (QString::compare(well_name, var->propertyInfo().parent_well_name) == 0) {
      well_controls.append(var);
    }
  }
  return well_controls;
}

// ---------------------------------------------------------
QList<ContinousProperty *>
VariablePropertyContainer::GetWellBHPVariables() const {

  QList<ContinousProperty *> bhp_variables;
  for (auto var : GetWellControlVariables()) {
    if (var->propertyInfo().prop_type == Property::PropertyType::BHP)
      bhp_variables.append(var);
  }
  return bhp_variables;
}

// ---------------------------------------------------------
QList<ContinousProperty *>
VariablePropertyContainer::GetWellRateVariables() const {

  QList<ContinousProperty *> rate_variables;
  for (auto var : GetWellControlVariables()) {
    if (var->propertyInfo().prop_type == Property::PropertyType::Rate)
      rate_variables.append(var);
  }
  return rate_variables;
}

// ---------------------------------------------------------
QList<ContinousProperty *>
VariablePropertyContainer::GetWellBHPVariables(QString well_name) const {

  QList<ContinousProperty *> well_bhp_variables;
  for (auto var : GetWellBHPVariables()) {
    if (QString::compare(well_name, var->propertyInfo().parent_well_name) == 0) {
      well_bhp_variables.append(var);
    }
  }
  return well_bhp_variables;
}

// ---------------------------------------------------------
QList<ContinousProperty *>
VariablePropertyContainer::GetWellRateVariables(QString well_name) const {

  QList<ContinousProperty *> well_rate_variables;
  for (auto var : GetWellRateVariables()) {
    if (QString::compare(well_name, var->propertyInfo().parent_well_name) == 0) {
      well_rate_variables.append(var);
    }
  }
  return well_rate_variables;
}

// ---------------------------------------------------------
QList<DiscreteProperty *>
VariablePropertyContainer::GetWellBlockVariables() const {

  QList<DiscreteProperty *> wb_vars;
  for (auto var : discrete_variables_->values()) {
    if (var->propertyInfo().prop_type == Property::PropertyType::WellBlock)
      wb_vars.append(var);
  }
  return wb_vars;
}

// ---------------------------------------------------------
QList<DiscreteProperty *>
VariablePropertyContainer::GetWellBlockVariables(
    const QString well_name) const {

  QList<DiscreteProperty *> wb_vars;
  for (auto var : GetWellBlockVariables()) {
    if (QString::compare(well_name, var->propertyInfo().parent_well_name) == 0)
      wb_vars.append(var);
  }
  return wb_vars;
}

// ---------------------------------------------------------
QList<ContinousProperty *>
VariablePropertyContainer::GetTransmissibilityVariables() const {
  QList<ContinousProperty *> trans_vars;
  for (auto var : continous_variables_->values()) {
    if (var->propertyInfo().prop_type == Property::PropertyType::Transmissibility)
      trans_vars.append(var);
  }
  return trans_vars;
}

// ---------------------------------------------------------
QList<ContinousProperty *>
VariablePropertyContainer::GetTransmissibilityVariables(
    const QString well_name) const {

  QList<ContinousProperty *> trans_vars;
  for (auto var : GetTransmissibilityVariables()) {
    if (QString::compare(well_name, var->propertyInfo().parent_well_name) == 0)
      trans_vars.append(var);
  }
  return trans_vars;
}

// ---------------------------------------------------------
BinaryProperty
*VariablePropertyContainer::GetBinaryVariable(QString name) const {

  for (auto var : binary_variables_->values()) {
    if (QString::compare(var->name(), name) == 0)
      return var;
  }
  throw std::runtime_error("Unable to find binary variable with name "
                               + name.toStdString());
}

// ---------------------------------------------------------
DiscreteProperty
*VariablePropertyContainer::GetDiscreteVariable(QString name) const {

  for (auto var : discrete_variables_->values()) {
    if (QString::compare(var->name(), name) == 0)
      return var;
  }
  throw std::runtime_error("Unable to find discrete variable with name "
                               + name.toStdString());
}

// ---------------------------------------------------------
ContinousProperty
*VariablePropertyContainer::GetContinousVariable(QString name) const {

  for (auto var : continous_variables_->values()) {
    if (QString::compare(var->name(), name) == 0)
      return var;
  }
  throw std::runtime_error("Unable to find continous variable with name "
                               + name.toStdString());
}

// ---------------------------------------------------------
QList<ContinousProperty *>
VariablePropertyContainer::GetContinuousProperties(QList<QUuid> ids) const {

  QList<ContinousProperty *> props;
  for (int i = 0; i < ids.size(); ++i) {
    props.append(continous_variables_->value(ids[i]));
  }
  return props;
}

}
}
