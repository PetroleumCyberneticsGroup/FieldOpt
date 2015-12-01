/******************************************************************************
 *
 *
 *
 * Created: 30.11.2015 2015 by einar
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

#include "case.h"
#include <limits>

namespace Optimization {

Case::Case() {
    binary_variables_ = QHash<QUuid, bool>();
    integer_variables_ = QHash<QUuid, int>();
    real_variables_ = QHash<QUuid, double>();
    objective_function_value_ = std::numeric_limits<double>::max();
}

Case::Case(const QHash<QUuid, bool> &binary_variables, const QHash<QUuid, int> &integer_variables, const QHash<QUuid, double> &real_variables)
{
    binary_variables_ = binary_variables;
    integer_variables_ = integer_variables;
    real_variables_ = real_variables;
    objective_function_value_ = std::numeric_limits<double>::max();
}

bool Case::Equals(const Case *other) const
{
    // Check if number of variables are equal
    if (this->binary_variables().size() != other->binary_variables().size()
            || this->integer_variables().size() != other->integer_variables().size()
            || this->real_variables().size() != other->real_variables().size())
        return false;
    foreach (QUuid key, this->binary_variables().keys()) {
        if (this->binary_variables()[key] != other->binary_variables()[key])
            return false;
    }
    foreach (QUuid key, this->integer_variables().keys()) {
        if (this->integer_variables()[key] != other->integer_variables()[key])
            return false;
    }
    foreach (QUuid key, this->real_variables().keys()) {
        if (this->real_variables()[key] != other->real_variables()[key])
            return false;
    }
    return true; // All variable values are equal if we reach this point.
}

double Case::objective_function_value() const {
    if (objective_function_value_ == std::numeric_limits<double>::max())
        throw ObjectiveFunctionException("The objective function value has not been set in this Case.");
    else
        return objective_function_value_;
}

}
