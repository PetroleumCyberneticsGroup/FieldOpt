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
#include <cmath>

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

Case::Case(const Case *c)
{
    binary_variables_ = QHash<QUuid, bool>(c->binary_variables());
    integer_variables_ = QHash<QUuid, int> (c->integer_variables());
    real_variables_ = QHash<QUuid, double> (c->real_variables());
    objective_function_value_ = c->objective_function_value_;
}

bool Case::Equals(const Case *other, double tolerance) const
{
    // Check if number of variables are equal
    if (this->binary_variables().size() != other->binary_variables().size()
            || this->integer_variables().size() != other->integer_variables().size()
            || this->real_variables().size() != other->real_variables().size())
        return false;
    foreach (QUuid key, this->binary_variables().keys()) {
        if (std::abs(this->binary_variables()[key] - other->binary_variables()[key]) > tolerance)
            return false;
    }
    foreach (QUuid key, this->integer_variables().keys()) {
        if (std::abs(this->integer_variables()[key] - other->integer_variables()[key]) > tolerance)
            return false;
    }
    foreach (QUuid key, this->real_variables().keys()) {
        if (std::abs(this->real_variables()[key] - other->real_variables()[key]) > tolerance)
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


void Case::set_integer_variable_value(const QUuid id, const int val)
{
    if (!integer_variables_.contains(id)) throw VariableException("Unable to set value of variable " + id.toString());
    integer_variables_[id] = val;
}

void Case::set_binary_variable_value(const QUuid id, const bool val)
{
    if (!binary_variables_.contains(id)) throw VariableException("Unable to set value of variable " + id.toString());
    binary_variables_[id] = val;
}

void Case::set_real_variable_value(const QUuid id, const double val)
{
    if (!real_variables_.contains(id)) throw VariableException("Unable to set value of variable " + id.toString());
    real_variables_[id] = val;
}

QList<Case *> Case::Perturb(QUuid variabe_id, Case::SIGN sign, double magnitude)
{
    QList<Case *> new_cases = QList<Case *>();
    if (this->integer_variables().contains(variabe_id)) {
        if (sign == PLUS || sign == PLUSMINUS) {
            Case *new_case_p = new Case(this);
            new_case_p->integer_variables_[variabe_id] += magnitude;
            new_case_p->objective_function_value_ = std::numeric_limits<double>::max();
            new_cases.append(new_case_p);
        }
        if (sign == MINUS || sign == PLUSMINUS) {
            Case *new_case_m = new Case(this);
            new_case_m->integer_variables_[variabe_id] -= magnitude;
            new_case_m->objective_function_value_ = std::numeric_limits<double>::max();
            new_cases.append(new_case_m);
        }
    } else if (real_variables_.contains(variabe_id)) {
        if (sign == PLUS || sign == PLUSMINUS) {
            Case *new_case_p = new Case(this);
            new_case_p->real_variables_[variabe_id] += magnitude;
            new_case_p->objective_function_value_ = std::numeric_limits<double>::max();
            new_cases.append(new_case_p);
        }
        if (sign == MINUS || sign == PLUSMINUS) {
            Case *new_case_m = new Case(this);
            new_case_m->real_variables_[variabe_id] -= magnitude;
            new_case_m->objective_function_value_ = std::numeric_limits<double>::max();
            new_cases.append(new_case_m);
        }
    }
    return new_cases;
}

}
