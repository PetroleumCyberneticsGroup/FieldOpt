/******************************************************************************
 *
 * realvariable.h
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

#ifndef REALVARIABLE_H
#define REALVARIABLE_H

#include "variable.h"

namespace Model {
namespace Variables {

class RealVariable : public Variable
{
public:
    RealVariable(double value);

    double value() const { return value_; }
    void setValue(double value);

    void Add(double d); //!< Add d to the value of this variable.

    /*!
     * \brief Equals checks whether the value of of this variable is equal to
     * the value of another variable, optionally within some tolerance.
     * \param other The variable to compare this to.
     * \param epsilon Optional tolerance. Default: 0.0
     * \return True if abs(this->value() - other->value()) <= epsilon; otherwise false.
     */
    bool Equals(RealVariable *other, double epsilon=0.0);

private:
    double value_;
};

}
}

#endif // REALVARIABLE_H
