/******************************************************************************
 *
 * variablecontainer.cpp
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

#include "variablecontainer.h"

namespace Model {
namespace Variables {

VariableContainer::VariableContainer()
{
    next_binary_id_ = 0;
    next_integer_id_ = 0;
    next_real_id_ = 0;

    binary_variables_ = new QHash<int, BinaryVariable *>();
    integer_variables_ = new QHash<int, IntegerVariable *>();
    real_variables_ = new QHash<int, RealVariable *>();
}

void VariableContainer::AddVariable(BinaryVariable *var)
{
    binary_variables_->insert(next_binary_id_++, var);
}

void VariableContainer::AddVariable(IntegerVariable *var)
{
    integer_variables_->insert(next_integer_id_++, var);
}

void VariableContainer::AddVariable(RealVariable *var)
{
    real_variables_->insert(next_real_id_++, var);
}

BinaryVariable *VariableContainer::GetBinaryVariable(int id) const
{
    if (!binary_variables_->contains(id)) throw VariableIdDoesNotExistException("Binary variable not found.");
    return binary_variables_->value(id);
}

IntegerVariable *VariableContainer::GetIntegerVariable(int id) const
{
    if (!integer_variables_->contains(id)) throw VariableIdDoesNotExistException("Integer variable not found.");
    return integer_variables_->value(id);
}

RealVariable *VariableContainer::GetRealVariable(int id) const
{
    if (!real_variables_->contains(id)) throw VariableIdDoesNotExistException("Real variable not found.");
    return real_variables_->value(id);
}

void VariableContainer::DeleteBinaryVariable(int id)
{
    if (!binary_variables_->contains(id)) throw VariableIdDoesNotExistException("Binary variable not found. Unable to delete.");
    binary_variables_->remove(id);
}

void VariableContainer::DeleteIntegerVariable(int id)
{
    if (!integer_variables_->contains(id)) throw VariableIdDoesNotExistException("Integer variable not found. Unable to delete.");
    integer_variables_->remove(id);
}

void VariableContainer::DeleteRealVariable(int id)
{
    if (!real_variables_->contains(id)) throw VariableIdDoesNotExistException("Real variable not found. Unable to delete");
    real_variables_->remove(id);
}


}
}
