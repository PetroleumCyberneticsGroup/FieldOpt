/******************************************************************************
 *
 * VariableContainer.cpp
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

#include "variable_property_container.h"

namespace Model {
namespace Properties {

VariablePropertyContainer::VariablePropertyContainer()
{
    next_binary_id_ = 0;
    next_discrete_id_ = 0;
    next_continous_id_ = 0;

    binary_variables_ = new QHash<int, BinaryProperty *>();
    discrete_variables_ = new QHash<int, DiscreteProperty *>();
    continous_variables_ = new QHash<int, ContinousProperty *>();
}

void VariablePropertyContainer::AddVariable(BinaryProperty *var)
{
    binary_variables_->insert(next_binary_id_++, var);
}

void VariablePropertyContainer::AddVariable(DiscreteProperty *var)
{
    discrete_variables_->insert(next_discrete_id_++, var);
}

void VariablePropertyContainer::AddVariable(ContinousProperty *var)
{
    continous_variables_->insert(next_continous_id_++, var);
}

BinaryProperty *VariablePropertyContainer::GetBinaryVariable(int id) const
{
    if (!binary_variables_->contains(id)) throw VariableIdDoesNotExistException("Binary variable not found.");
    return binary_variables_->value(id);
}

DiscreteProperty *VariablePropertyContainer::GetDiscreteVariable(int id) const
{
    if (!discrete_variables_->contains(id)) throw VariableIdDoesNotExistException("Integer variable not found.");
    return discrete_variables_->value(id);
}

ContinousProperty *VariablePropertyContainer::GetContinousVariable(int id) const
{
    if (!continous_variables_->contains(id)) throw VariableIdDoesNotExistException("Real variable not found.");
    return continous_variables_->value(id);
}

QHash<int, bool> VariablePropertyContainer::GetBinaryVariableValues() const
{
    QHash<int, bool> binary_values = QHash<int, bool>();
    foreach (int key, binary_variables_->keys())
        binary_values[key] = binary_variables_->value(key)->value();
    return binary_values;
}

QHash<int, int> VariablePropertyContainer::GetDiscreteVariableValues() const
{
    QHash<int, int> discrete_values = QHash<int, int>();
    foreach (int key, discrete_variables_->keys()) {
        discrete_values[key] = discrete_variables_->value(key)->value();
    }
    return discrete_values;
}

QHash<int, double> VariablePropertyContainer::GetContinousVariableValues() const
{
    QHash<int, double> continous_values = QHash<int, double>();
    foreach (int key, continous_variables_->keys()) {
        continous_values[key] = continous_variables_->value(key)->value();
    }
    return continous_values;
}

void VariablePropertyContainer::DeleteBinaryVariable(int id)
{
    if (!binary_variables_->contains(id)) throw VariableIdDoesNotExistException("Binary variable not found. Unable to delete.");
    binary_variables_->remove(id);
}

void VariablePropertyContainer::DeleteDiscreteVariable(int id)
{
    if (!discrete_variables_->contains(id)) throw VariableIdDoesNotExistException("Integer variable not found. Unable to delete.");
    discrete_variables_->remove(id);
}

void VariablePropertyContainer::DeleteContinousVariable(int id)
{
    if (!continous_variables_->contains(id)) throw VariableIdDoesNotExistException("Real variable not found. Unable to delete");
    continous_variables_->remove(id);
}


}
}
