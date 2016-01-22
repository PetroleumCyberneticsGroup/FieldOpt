/******************************************************************************
 *
 * integervariable.cpp
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

#include "discrete_property.h"
#include "property_exceptions.h"

namespace Model {
namespace Properties {

DiscreteProperty::DiscreteProperty(int value)
    : Property(Discrete)
{
    value_ = value;
}

void DiscreteProperty::setValue(int value)
{
    if (IsLocked()) throw PropertyLockedException("Cant change locked integer variable.");
    else value_ = value;
}

void DiscreteProperty::Add(int i)
{
    if (IsLocked()) throw PropertyLockedException("Cant add to a locked integer variable");
    else value_ += i;
}

bool DiscreteProperty::Equals(const DiscreteProperty *other) const
{
    if (this->value() == other->value()) return true;
    else return false;
}

QString DiscreteProperty::ToString() const
{
    return QString("Type:\tDiscrete\nUUID:\t%1\nName:\t%2\nValue:\t%3\n").arg(id().toString()).arg(name()).arg(value());
}




}
}
