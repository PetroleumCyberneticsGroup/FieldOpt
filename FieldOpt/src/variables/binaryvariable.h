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

#ifndef BINARYVARIABLE_H
#define BINARYVARIABLE_H

#include "variable.h"

/*!
 * \brief Class for binary variables.
 *
 * The value should, in principle be 0 or 1, but certain algorithms need to vary it in the interval between 0.0 and 1.0.
 *
 * \todo Check that min/max bounds make sense when set. E.g. max > min
 */
class BinaryVariable : public Variable
{
private:
    double m_value; //!< The current value for this variable.
    bool m_is_variable; //!< Determines whether this variable is variable or not.

public:
    BinaryVariable(Component *parent);  //!< Calls the Variable constructor.

    /*!
     * \brief Set the current value for this variable.
     *
     * Checks that the new value is within the bounds.
     * For binary variables, the bounds are always 0.0 and 1.0.
     * Emits the warning signal if the parameter is greater/less than the bounds.
     * \param value The new value to be set.
     */
    void setValue(double value);
    void setIsVariable(bool b) { m_is_variable = b; } //!< Set whether this variable may be varied

    double value() { return m_value; }          //!< Get function for m_value
    double min() { return 0.0; }                //!< Min value for all binary variables.
    double max() { return 1.0; }                //!< Max value for all binary variables.
    virtual bool isVariable() { return m_is_variable; } //!< Get fuction for m_is_variable
};

#endif // BINARYVARIABLE_H
