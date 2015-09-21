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

#ifndef DERIVATIVE_H
#define DERIVATIVE_H

#include <QPair>
#include <QVector>

/*!
 * \brief Describes the derivative of a variable.
 */
class Derivative
{
private:
    int m_constraint_id;
    QVector<QPair<int, double> > m_partial_derivatives;

public:
    Derivative();            //!< Initializes m_constraint_id to -1.
    Derivative(int con_id);  //!< Initializes m_constraint_id to con_id.

    void addPartial(int var_id, double value) {m_partial_derivatives.push_back(QPair<int, double>(var_id, value));}  //!< Add a new partial derivative to this derivative.

    int constraintId() {return m_constraint_id;}                      //!< Get the constriant id.
    int numberOfPartials() {return m_partial_derivatives.size();}     //!< Get the number of partial derivatives.
    double valueById(int var_id);                                     //!< Get the value of the partial derivative for variable with id = var_id. Returns 0 if not found.
    double value(int i) {return m_partial_derivatives.at(i).second;}  //!< Get the value at position i.
};

#endif // DERIVATIVE_H
