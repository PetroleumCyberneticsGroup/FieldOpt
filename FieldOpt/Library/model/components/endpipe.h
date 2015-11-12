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

#ifndef ENDPIPE_H
#define ENDPIPE_H

#include "Library/model/stream.h"
#include "Library/model/components/pipe.h"

/*!
 * \brief A top node in the Pipe network.
 *
 * An EndPipe is the last downstream component in the pipe network. It does not have any outlet connections.
 *
 * A minimum outlet pressure is specified for the EndPipe. The minimum outlet pressure works as a boundary
 * condition for the pipe network, and is used by upstream parts of the network to calculate inlet pipe
 * pressures, all the way down to the production wells.
 *
 * The inlet pressure of the pipes directly connected to production wells are compared to the producing
 * bottomhole pressure of the well. If the bottomhole pressure is lower than the pipe inlet pressure, the
 * solution is not feasible. If the bottomhole pressure is higher, the solution is considered feasible.
 * It is assumed that the well may be choked back to correspond to the pipe pressure.
 */
class EndPipe : public Pipe
{
private:
    double m_outletpressure;
    Stream::units m_outlet_unit;
public:
    EndPipe();                  //!< Default constructor. Sets outlet pressure to -1.0 and units to FIELD.
    EndPipe(const EndPipe &p);  //!< Copy constructor.
    virtual ~EndPipe() {}       //!< Default destructor.

    virtual Pipe* clone() {return new EndPipe(*this);}  //!< Get a copy of this pipe.
    virtual void calculateInletPressure();              //!< Calculate the inlet preesure of this pipe.
    virtual QString description() const;                //!< Generate a description of this pipe for the driver file.

    void setOutletPressure(double p) {m_outletpressure = p;}  //!< Set the outlet pressure of the pipe.
    void setOutletUnit(Stream::units u) {m_outlet_unit = u;}  //!< Set the outlet units of the pipe.

    double outletPressure() const {return m_outletpressure;}  //!< Get the outlet pressure of the pipe.
    Stream::units outletUnit() const {return m_outlet_unit;}  //!< Get the outlet units ofthe pipe.
};

#endif // ENDPIPE_H
