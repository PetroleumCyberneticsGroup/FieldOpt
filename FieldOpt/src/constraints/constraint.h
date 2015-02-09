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

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <QString>

/*!
 * \brief Class for constraints.
 *
 * TODO: This class will be significantly more advanced.
 *
 * TODO: Check that min/max bounds make sense when set. E.g. max > min
 */
class Constraint
{
private:
    double m_value;      //!< Current value of this contraint.
    double m_max;        //!< Maximum value for this constraint.
    double m_min;        //!< Minimum value for this contraint.
    QString m_name;      //!< Name of this contraint.
    int m_id;            //!< Unique id for this contraint.

    static int next_id;  //!< Next id to be assigned. Initialized to 0.

public:
    /*!
     * \brief Initialize with default values.
     *
     * Set m_id to next_id and increment next_id. Initialize m_value, m_max and m_min to 0.
     */
    Constraint();

    /*!
     * \brief Initialize with provided values.
     * \param value Value to be set.
     * \param max Maximum bound to be set.
     * \param min Minumum bound to be set.
     */
    Constraint(double value, double max, double min);

    void setName(const QString name) { m_name = name; }  //!< Set m_name.
    void setValue(double value) { m_value = value; }     //!< Set m_value.
    void setMax(double max) { m_max = max; }             //!< Set m_max.
    void setMin(double min) { m_min = min; }             //!< Set m_min

    const QString& name() { return m_name; }  //!< Get function for m_name.
    double value() {return m_value; }         //!< Get function for m_value.
    double max() {return m_max; }             //!< Get function for m_max.
    double min() {return m_min; }             //!< Get function for m_min.
    int id() {return m_id; }                  //!< Get function for m_id.

};

#endif // CONSTRAINT_H
