/******************************************************************************
 *
 * integervariable.h
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

#ifndef INTEGERVARIABLE_H
#define INTEGERVARIABLE_H

#include "variable.h"

namespace Model {
namespace Variables {

/*!
 * \brief The IntegerVariable class describes integer variables, commonly used when optimizing
 * for example well placement in a discrete grid.
 */
class IntegerVariable : public Variable
{
public:
    IntegerVariable(int value);

    int value() const { return value_; }
    void setValue(int value);

    void Add(int i); //!< Add i to the value of this variable.
    bool Equals(const IntegerVariable *other) const; // Check if the value of another variable equals the value of this variable.

private:
    int value_;
};

}
}
#endif // INTEGERVARIABLE_H
