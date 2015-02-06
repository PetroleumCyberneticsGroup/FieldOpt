/*
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einarjba@stud.ntnu.no>
 *
 * The code is largely based on ResOpt, created by  Aleksander O. Juell <aleksander.juell@ntnu.no>
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
 */

#include "realvariable.h"
#include <limits>

RealVariable::RealVariable() : Variable()
{
    m_max = std::numeric_limits<double>::max();
    m_min = std::numeric_limits<double>::min();
}

void RealVariable::setValue(double value)
{
    if (value > m_max)
    {
        m_value = m_max;
        QString message = QString("Value %1 is greater than the set upper bound: %2.\n\tThe value has been set equal to the upper bound.").arg(value).arg(m_max);
        emitException(ExceptionSeverity::WARNING, ExceptionType::OUT_OF_BOUNDS, message);
    }
    else if (value < m_min) {
        m_value = m_min;
        QString message = QString("Value %1 is less than the set lower bound: %2.\n\tThe value has been set equal to the lower bound.").arg(value).arg(m_max);
        emitException(ExceptionSeverity::WARNING, ExceptionType::OUT_OF_BOUNDS, message);
    }
    else
    {
        m_value = value;
    }
}

bool RealVariable::isVariable()
{
    if(max() == value() && min() == value())
        return false;
    else
        return true;
}
