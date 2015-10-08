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

#ifndef MATERIALBALANCECONSTRAINT_H
#define MATERIALBALANCECONSTRAINT_H

#include <tr1/memory>
#include "Library/model/stream.h"
#include "Library/model/inputratevariable.h"
#include "Library/variables/realvariable.h"
#include "Library/model/components/pipe.h"
#include "constraint.h"

class InputRateVariable;
class Constraint;

using std::tr1::shared_ptr;

/*!
 * \brief The constraints associated with an InputRateVariable in a DecoupledModel.
 *
 * \todo Should extend Constraint?
 */
class MaterialBalanceConstraint
{
private:
    Stream m_stream;                     //!< The stream containing rates that the input rate variable must be equal to for mass balance feasibility
    InputRateVariable *p_irv;            //!< The InputRateVariable for the stream.
    shared_ptr<Constraint> p_con_oil;    //!< Oil constraint.
    shared_ptr<Constraint> p_con_gas;    //!< Gas constraint.
    shared_ptr<Constraint> p_con_water;  //!< Water constrainte.

public:
    MaterialBalanceConstraint();  //!< Sets all variables to 0.

    bool updateConstraints();  //!< Update the value of the oil, gas, and water constraint according to the values in the stream and the input rate variable.
    void emptyStream();  //!< Empty the Stream by settings all rates to 0.0.

    void setStream(const Stream &s) {m_stream = s;}     //!< Set the Stream.
    void setInputRateVariable(InputRateVariable *irv);  //!< Set the InputRateVariable and assign names to the constraints.

    shared_ptr<Constraint> oilConstraint() {return p_con_oil;}      //!< Get the oil Constraint.
    shared_ptr<Constraint> gasConstraint() {return p_con_gas;}      //!< Get the gas Constraint.
    shared_ptr<Constraint> waterConstraint() {return p_con_water;}  //!< Get the water Constraint.
    InputRateVariable* inputRateVariable() {return p_irv;}          //!< Get the InputRateVariable.
    const Stream& stream() const {return m_stream;}                 //!< Get the Stream.
};

#endif // MATERIALBALANCECONSTRAINT_H
