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

#ifndef INPUTRATEVARIABLE_H
#define INPUTRATEVARIABLE_H

#include <tr1/memory>

#include "components/pipe.h"
#include "stream.h"
#include "Library/variables/realvariable.h"

class Pipe;
class Stream;
class RealVariable;

using std::tr1::shared_ptr;


/*!
 * \brief Connects a set of three RealVariables to a Stream.
 *
 * This class is used by DecoupledModel to connect variables to streams in the different parts of the model.
 */
class InputRateVariable
{
private:
    Pipe *p_pipe;                          //!< The pipe this variable belongs to.
    Stream *p_stream;                      //!< The stream which passes through the pipe.
    shared_ptr<RealVariable> p_var_oil;    //!< Oil variable.
    shared_ptr<RealVariable> p_var_gas;    //!< Gas variable.
    shared_ptr<RealVariable> p_var_water;  //!< Water variable.

public:
    InputRateVariable();  //!< Default constructor. Initializes the stream to 0.

    bool updateStream();  //!< Update the stream with the current values for the varaibles. Returns false if either of the variables are undefined.

    void setPipe(Pipe *p) {p_pipe = p;}                                   //!< Set the Pipe.
    void setStream(Stream *s) {p_stream = s;}                             //!< Set the Stream.
    void setOilVariable(shared_ptr<RealVariable> v) {p_var_oil = v;}      //!< Set the oil Variable.
    void setGasVariable(shared_ptr<RealVariable> v) {p_var_gas = v;}      //!< Set the gas Variable.
    void setWaterVariable(shared_ptr<RealVariable> v) {p_var_water = v;}  //!< Set the water Variable.

    Pipe* pipe() {return p_pipe;}                                   //!< Get the Pipe.
    Stream* stream() {return p_stream;}                             //!< Get the Stream.
    shared_ptr<RealVariable> oilVariable() {return p_var_oil;}      //!< Get the oil Variable.
    shared_ptr<RealVariable> gasVariable() {return p_var_gas;}      //!< Get the gas Variable.
    shared_ptr<RealVariable> waterVariable() {return p_var_water;}  //!< Get the water Variable.
};

#endif // INPUTRATEVARIABLE_H
