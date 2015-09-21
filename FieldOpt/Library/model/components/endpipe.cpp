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

#include "endpipe.h"

EndPipe::EndPipe() :
    m_outletpressure(-1.0),
    m_outlet_unit(Stream::FIELD)
{}

EndPipe::EndPipe(const EndPipe &p) :
    Pipe(p)
{
    m_outletpressure = p.m_outletpressure;
    m_outlet_unit = p.m_outlet_unit;
}

void EndPipe::calculateInletPressure()
{
    for(int i = 0; i < numberOfStreams(); i++)
    {
        // Check that the outlet pressure unit matches the stream. If not, convert.
        double out_pres = outletPressure();
        if(outletUnit() != stream(i)->inputUnits())
        {
            if(outletUnit() == Stream::METRIC)
                out_pres = outletPressure()*14.5037738;
            else
                out_pres = outletPressure() / 14.5037738;
        }
        double dp = calculator()->pressureDrop(stream(i), outletPressure(), stream(i)->inputUnits());
        stream(i)->setPressure(dp + outletPressure());
    }
}

QString EndPipe::description() const
{
    QString str("START PIPE\n");
    str.append(" NUMBER " + QString::number(number()) + "\n");
    str.append(" FILE " + fileName() + "\n");
    str.append(" OUTLETPRESSURE " + QString::number(outletPressure()));
    if(outletUnit() == Stream::METRIC)
        str.append(" BARA\n");
    else
        str.append(" PSIA\n");

    str.append("END PIPE\n\n");
    return str;
}
