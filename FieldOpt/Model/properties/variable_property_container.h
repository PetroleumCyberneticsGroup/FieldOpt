/******************************************************************************
 *
 * VariableContainer.h
 *
 * Created: 23.09.2015 2015 by einar
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
public:
    VariablePropertyContainer();

    void AddVariable(BinaryProperty *var);
    void AddVariable(DiscreteProperty *var);
    void AddVariable(ContinousProperty *var);

    int BinaryVariableSize() const { return binary_variables_->size(); } //!< Get the number of binary variables.
    int DiscreteVariableSize() const { return discrete_variables_->size(); } //!< Get the number of discrete variables.
    int ContinousVariableSize() const { return continous_variables_->size(); } //!< Get the number of continous variables.

    BinaryProperty *GetBinaryVariable(QUuid id) const; //!< Get the binary variable with index id.
    DiscreteProperty *GetDiscreteVariable(QUuid id) const; //!< Get the discrete variable with index id.
    ContinousProperty *GetContinousVariable(QUuid id) const; //!< Get the continous variable with index id.
    void SetBinaryVariableValue(QUuid id, bool val); //!< Set the value of a binary variable.
    void SetDiscreteVariableValue(QUuid id, int val); //!< Set the value of a binary variable.
    void SetContinousVariableValue(QUuid id, double val); //!< Set the value of a binary variable.
    QHash<QUuid, BinaryProperty *> *GetBinaryVariables() const { return binary_variables_; } //!< Get all binary variables
    QHash<QUuid, DiscreteProperty *> *GetDiscreteVariables() const { return discrete_variables_; } //!< Get all discrete variables
    QHash<QUuid, ContinousProperty *> *GetContinousVariables() const { return continous_variables_; } //!< Get all continous variables
    QHash<QUuid, bool> GetBinaryVariableValues() const; //!< Get a hashmap containing all binary variable values. The key represents each variable's ID.
    QHash<QUuid, int> GetDiscreteVariableValues() const; //!< Get a hashmap containing all discrete variable values. The key represents each variable's ID.
    QHash<QUuid, double> GetContinousVariableValues() const; //!< Get a hashmap containing all discrete varaible values. The key represents each variable's ID.
    QList<QUuid> GetBinaryVariableIdsWithName(QString var_name) const;
    QList<QUuid> GetDiscreteVariableIdsWithName(QString var_name) const;
    QList<QUuid> GetContinousVariableIdsWithName(QString var_name) const;

    /*!
     * \brief GetContinousVariableNamesAndIdsMatchingNamePart Get the names and IDs of continous variables with
     * names containing the proided substring.
     * \param substring
     */
    QList<QPair<QUuid, QString> > GetContinousVariableNamesAndIdsMatchingSubstring(QString substring);

    void DeleteBinaryVariable(QUuid id);
    void DeleteDiscreteVariable(QUuid id);
    void DeleteContinousVariable(QUuid id);

    void CheckVariableNameUniqueness(); //!< Check that all variable names are unique. If they are not, throw an error.

private:
    QHash<QUuid, BinaryProperty *> *binary_variables_;
    QHash<QUuid, DiscreteProperty *> *discrete_variables_;
    QHash<QUuid, ContinousProperty *> *continous_variables_;
};

}
}

#endif // VARIABLE_PROPERTY_CONTAINER_H
