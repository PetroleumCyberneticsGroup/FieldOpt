/******************************************************************************
 *
 * variablecontainer.h
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

#ifndef VARIABLECONTAINER_H
#define VARIABLECONTAINER_H

#include <QHash>

#include "binaryvariable.h"
#include "integervariable.h"
#include "realvariable.h"

namespace Model {
namespace Variables {

/*!
 * \brief The VariableContainer class simplifies the handling of variables.
 *
 * This class has members that hold all integer, real and binary varialbes in the model
 * and methods to maintain the lists.
 */
class VariableContainer
{
public:
    VariableContainer();

    void AddVariable(BinaryVariable *var);
    void AddVariable(IntegerVariable *var);
    void AddVariable(RealVariable *var);

    int BinaryVariableSize() const { return binary_variables_->size(); } //!< Get the number of binary variables.
    int IntegerVariableSize() const { return integer_variables_->size(); } //!< Get the number of binary variables.
    int RealVariableSize() const { return real_variables_->size(); } //!< Get the number of binary variables.

    BinaryVariable *GetBinaryVariable(int id) const; //!< Get the binary variable with index id.
    IntegerVariable *GetIntegerVariable(int id) const; //!< Get the integer variable with index id.
    RealVariable *GetRealVariable(int id) const; //!< Get the real variable with index id.

    void DeleteBinaryVariable(int id);
    void DeleteIntegerVariable(int id);
    void DeleteRealVariable(int id);

private:
    int next_binary_id_;
    int next_integer_id_;
    int next_real_id_;

    QHash<int, BinaryVariable *> *binary_variables_;
    QHash<int, IntegerVariable *> *integer_variables_;
    QHash<int, RealVariable *> *real_variables_;
};

}
}

#endif // VARIABLECONTAINER_H
