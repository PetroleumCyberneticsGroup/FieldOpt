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

#ifndef PIPECONNECTION_H
#define PIPECONNECTION_H

#include <tr1/memory>
#include "Library/model/components/pipe.h"
#include "Library/variables/binaryvariable.h"

using std::tr1::shared_ptr;

class Pipe;
class BinaryVariable;

/*!
 * \brief Defines a connection from a Well or Pipe to a Pipe.
 */
class PipeConnection
{
private:
    int m_pipe_number;                              //!< Identifying number for the pipe.
    Pipe* p_pipe;                                   //!< Pointer to the pipe being connected to.
    shared_ptr<BinaryVariable> p_routing_variable;  //!< \todo Document this.

public:
    PipeConnection();                         //!< Default constructor. Sets m_pipe_number to -1 and p_pipe to 0.
    PipeConnection(const PipeConnection &p);  //!< Copy constructor.
    ~PipeConnection() {}                      //!< Default destructor.

    void setPipeNumber(int i) {m_pipe_number = i;}                            //!< Set function for m_pipe_number.
    void setPipe(Pipe *p) {p_pipe = p;}                                       //!< Set function for p_pipe.
    void setVariable(shared_ptr<BinaryVariable> v) {p_routing_variable = v;}  //!< Set function for p_routing_variable.

    int pipeNumber() const {return m_pipe_number;}                      //!< Get m_pipe_number.
    Pipe* pipe() {return p_pipe;}                                       //!< Get p_pipe.
    shared_ptr<BinaryVariable> variable() {return p_routing_variable;}  //!< Get p_routing_variable.
};

#endif // PIPECONNECTION_H
