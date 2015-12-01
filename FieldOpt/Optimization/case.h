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

#ifndef CASE_H
#define CASE_H

#include <QHash>
#include <QUuid>
#include "optimization_exceptions.h"

namespace Optimization {

class CaseHandler;

/*!
 * \brief The Case class represents a specific case for the optimizer, i.e. a specific set of variable values
 * and the value of the objective function after evaluation.
 */
class Case
{
public:
    friend class CaseHandler;
    Case();
    Case(const QHash<QUuid, bool> &binary_variables, const QHash<QUuid, int> &integer_variables, const QHash<QUuid, double> &real_variables);
    Case(const Case &c) = delete;

    /*!
     * \brief Equals Checks whether this case is equal to another case.
     *
     * To cases are considered equal if they contain the same variable values.
     * \param other Case to compare with.
     * \return True if the cases contain the same varaible values; otherwise false;
     */
    bool Equals(const Case *other) const;

    QUuid id() const { return id_; }

    QHash<QUuid, bool> binary_variables() const { return binary_variables_; }
    QHash<QUuid, int> integer_variables() const { return integer_variables_; }
    QHash<QUuid, double> real_variables() const { return real_variables_; }
    void set_binary_variables(const QHash<QUuid, bool> &binary_variables) { binary_variables_ = binary_variables; }
    void set_integer_variables(const QHash<QUuid, int> &integer_variables) { integer_variables_ = integer_variables; }
    void set_real_variables(const QHash<QUuid, double> &real_variables) { real_variables_ = real_variables; }

    double objective_function_value() const; //!< Get the objective function value. Throws an exception if the value has not been defined.
    void set_objective_function_value(double objective_function_value) { objective_function_value_ = objective_function_value; }


private:
    QUuid id_ = QUuid::createUuid(); //!< Unique ID for the case.

    double objective_function_value_;
    QHash<QUuid, bool> binary_variables_;
    QHash<QUuid, int> integer_variables_;
    QHash<QUuid, double> real_variables_;

};

}

#endif // CASE_H
