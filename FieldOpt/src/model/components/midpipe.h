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

#ifndef MIDPIPE_H
#define MIDPIPE_H

#include <tr1/memory>
#include "pipe.h"
#include "variables/intvariable.h"
#include "model/pipeconnection.h"
#include "constraints/constraint.h"

class IntVariable;
class PipeConnection;
class Constraint;

using std::tr1::shared_ptr;

/*!
 * \brief A pipe that has both input- and output connections.
 */
class MidPipe : public Pipe
{
private:
    QVector<PipeConnection*> m_outlet_connections;   //!< Connections for this pipe.
    shared_ptr<Constraint> p_connection_constraint;  //!< Constraint that makes sure that the sum of flow to pipes equals 1.

public:
    MidPipe();                  //!< Default constructor. Initializes the connection constraint.
    MidPipe(const MidPipe &p);  //!< Copy constructor.
    virtual ~MidPipe();         //!< Default destructor. Deletes the outlet connections.

    virtual Pipe* clone() {return new MidPipe(*this);}  //!< Get a clone of this object.
    virtual void calculateInletPressure();              //!< Calculate the inlet pressure of this pipe. Exit the program if the model is inconsistent.
    virtual QString description() const;                //!< Generate a description for the driver file.

    void setNumber(int n);  //!< Overloaded from the Pipe class. Sets the pipe number and updates the name in the connection constraint.

    void updateOutletConnectionConstraint();                   //!< Update the outlet connections constraint.
    void setMustRoute() {p_connection_constraint->setMin(1);}  //!< Set the connection constraint so that this pipe must be routed to _one_ outlet connection.

    bool midpipeConnectedUpstream();  //!< Check whether any of the outlet connections are of type MidPipe.

    void addOutletConnection(PipeConnection *c) {m_outlet_connections.push_back(c);}  //!< Add an outlet connection.

    int numberOfOutletConnections() const {return m_outlet_connections.size();}            //!< Get the number of outlet connections.
    PipeConnection* outletConnection(int i) {return m_outlet_connections.at(i);}           //!< Get outlet connection i.
    shared_ptr<Constraint> outletConnectionConstraint() {return p_connection_constraint;}  //!< Get the outlet connection constraint.

    /*!
     * \brief Find the fraction of the rates from this Pipe that go through upstream_pipe
     * \param upstream_pipe
     * \param ok false if the upstream_pipe is not connected to this pipe.
     * \return The fraction of the rates from this Pipe that passes througn an upstream pipe.
     */
    double flowFraction(Pipe *upstream_pipe, bool *ok = 0);
};

#endif // MIDPIPE_H
