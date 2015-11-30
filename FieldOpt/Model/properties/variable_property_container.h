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

    BinaryProperty *GetBinaryVariable(int id) const; //!< Get the binary variable with index id.
    DiscreteProperty *GetDiscreteVariable(int id) const; //!< Get the discrete variable with index id.
    ContinousProperty *GetContinousVariable(int id) const; //!< Get the continous variable with index id.
    QHash<int, bool> GetBinaryVariableValues() const; //!< Get a hashmap containing all binary variable values. The key represents each variable's ID.
    QHash<int, int> GetDiscreteVariableValues() const; //!< Get a hashmap containing all discrete variable values. The key represents each variable's ID.
    QHash<int, double> GetContinousVariableValues() const; //!< Get a hashmap containing all discrete varaible values. The key represents each variable's ID.

    void DeleteBinaryVariable(int id);
    void DeleteDiscreteVariable(int id);
    void DeleteContinousVariable(int id);

private:
    int next_binary_id_;
    int next_discrete_id_;
    int next_continous_id_;

    QHash<int, BinaryProperty *> *binary_variables_;
    QHash<int, DiscreteProperty *> *discrete_variables_;
    QHash<int, ContinousProperty *> *continous_variables_;
};

}
}

#endif // VARIABLE_PROPERTY_CONTAINER_H
