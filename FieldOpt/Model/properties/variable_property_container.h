/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#ifndef VARIABLE_PROPERTY_CONTAINER_H
#define VARIABLE_PROPERTY_CONTAINER_H

#include <QHash>
#include <QPair>
#include <QString>

#include "property.h"
#include "binary_property.h"
#include "discrete_property.h"
#include "continous_property.h"

namespace Model {
class ModelSynchronizationObject;
}

namespace Model {
namespace Properties {

/*!
 * \brief The VariablePropertyContainer class facilitates the handling of variable properties.
 *
 * This class has members that hold all continous, discrete and binary properties that are
 * considered _variable_ in the model, as well as and methods to maintain the lists.
 *
 * This class also maintains unique IDs in the form of hashtable keys for every variable property
 * of each type.
 */
class VariablePropertyContainer
{
  friend class ::Model::ModelSynchronizationObject;
 public:
  VariablePropertyContainer();

  void AddVariable(BinaryProperty *var); //!< Add a property to the container and mark it as variable
  void AddVariable(DiscreteProperty *var); //!< Add a property to the container and mark it as variable
  void AddVariable(ContinousProperty *var); //!< Add a property to the container and mark it as variable

  int BinaryVariableSize() const { return binary_variables_->size(); } //!< Get the number of binary variables.
  int DiscreteVariableSize() const { return discrete_variables_->size(); } //!< Get the number of discrete variables.
  int ContinousVariableSize() const { return continous_variables_->size(); } //!< Get the number of continous variables.

  BinaryProperty *GetBinaryVariable(QUuid id) const; //!< Get the binary variable with index id.
  DiscreteProperty *GetDiscreteVariable(QUuid id) const; //!< Get the discrete variable with index id.
  ContinousProperty *GetContinousVariable(QUuid id) const; //!< Get the continous variable with index id.

  BinaryProperty *GetBinaryVariable(QString name) const; //!< Get the binary variable with the specified name.
  DiscreteProperty *GetDiscreteVariable(QString name) const; //!< Get the discrete variable with the specified name.
  ContinousProperty *GetContinousVariable(QString name) const; //!< Get the continous variable with the specified name.

  void SetBinaryVariableValue(QUuid id, bool val); //!< Set the value of a binary variable.
  void SetDiscreteVariableValue(QUuid id, int val); //!< Set the value of a binary variable.

  void SetContinousVariableValue(QUuid id, double val); //!< Set the value of a binary variable.
  QHash<QUuid, BinaryProperty *> *GetBinaryVariables() const { return binary_variables_; } //!< Get all binary variables
  QHash<QUuid, DiscreteProperty *> *GetDiscreteVariables() const { return discrete_variables_; } //!< Get all discrete variables

  QHash<QUuid, ContinousProperty *> *GetContinousVariables() const { return continous_variables_; } //!< Get all continous variables
  QHash<QUuid, bool> GetBinaryVariableValues() const; //!< Get a hashmap containing all binary variable values. The key represents each variable's ID.
  QHash<QUuid, int> GetDiscreteVariableValues() const; //!< Get a hashmap containing all discrete variable values. The key represents each variable's ID.

  QHash<QUuid, double> GetContinousVariableValues() const; //!< Get a hashmap containing all discrete varaible values. The key represents each variable's ID.

  QList<ContinousProperty *> GetWellControlVariables() const; //!< Get all control (rate/bhp) variables.
  QList<ContinousProperty *> GetWellBHPVariables() const; //!< Get all BHP variables.
  QList<ContinousProperty *> GetWellRateVariables() const ; //!< Get all BHP variables.
  QList<ContinousProperty *> GetWellControlVariables(const QString well_name) const; //!< Get all control variables for a specific well
  QList<ContinousProperty *> GetWellBHPVariables(const QString well_name) const; //!< Get all BHP variables for a specific well.
  QList<ContinousProperty *> GetWellRateVariables(const QString well_name) const; //!< Get all BHP variables for a specific well.
  QList<ContinousProperty *> GetWellSplineVariables(const QString well_name) const; //!< Get all variables for the spline defining a well.
  QList<ContinousProperty *> GetAWPSplineVariables(const QString well_name) const; //!< Get all variables for the spline defining a well.
  QList<ContinousProperty *> GetPolarSplineVariables(const QString well_name) const; //!< Get all variables defining a polar well spline.
  QList<DiscreteProperty *> GetWellBlockVariables() const; //!< Get well block position variables.
  QList<DiscreteProperty *> GetWellBlockVariables(const QString well_name) const; //!< Get well block position variables for a well.
  QList<ContinousProperty *> GetPseudoContVertVariables(const QString well_name) const; //!< Get x and y pseudo-continuous vertical well variables for a well. First element is x; second is y.
  QList<ContinousProperty *> GetTransmissibilityVariables() const; //!< Get all transmissibility variables.
  QList<ContinousProperty *> GetTransmissibilityVariables(const QString well_name) const; //!< Get all transmissibility variables for a well.

  /*!
   * @brief Get a list of properties in the same order as they occur in the
   * input ID vector.
   * @param ids IDs for properties to get.
   * @return A vector of properties.
   */
  QList<ContinousProperty *> GetContinuousProperties(QList<QUuid> ids) const;

  void CheckVariableNameUniqueness(); //!< Check that all variable names are unique. If they are not, throw an error.

 private:
  QHash<QUuid, BinaryProperty *> *binary_variables_;
  QHash<QUuid, DiscreteProperty *> *discrete_variables_;
  QHash<QUuid, ContinousProperty *> *continous_variables_;
};

}
}

#endif // VARIABLE_PROPERTY_CONTAINER_H
