/******************************************************************************
 *
 * realvariable.cpp
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

#include "realvariable.h"
#include <cmath>

namespace Model {
namespace Variables {

RealVariable::RealVariable(double value)
    : Variable(Real)
{
    value_ = value;
}

void RealVariable::setValue(double value)
{
    if (IsLocked()) throw VariableLockedException("Cant change locked real variable.");
    else value_ = value;
}

void RealVariable::Add(double d)
{
    if (IsLocked()) throw VariableLockedException("Cant add to locked real variable");
    else value_ += d;
}

bool RealVariable::Equals(const RealVariable *other, double epsilon) const
{
    return std::abs(this->value() - other->value()) <= epsilon;
}



}
}
