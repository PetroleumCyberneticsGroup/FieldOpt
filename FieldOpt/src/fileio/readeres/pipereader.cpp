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

#include "pipereader.h"

BeggsBrillCalculator *PipeReader::readBeggsBrill(QFile &file)
{
    BeggsBrillCalculator* bb = new BeggsBrillCalculator();

    double l_diameter = 0.0;
    double l_length = 0.0;
    double l_angle = 0.0;
    double l_temperature = 0.0;
    double l_sg_gas = 0.0;
    double l_den_oil = 0.0;
    double l_den_wat = 0.0;
    double l_vis_oil = 0.0;
    double l_vis_wat = 0.0;
    bool ok = true;

    QStringList list = StringUtilities::processLine(file.readLine());
    while(!file.atEnd() && !list.at(0).startsWith("EOF"))
    {
        if(list.at(0).startsWith("DIAMETER")) l_diameter = list.at(1).toDouble(&ok);            // getting the diameter
        else if(list.at(0).startsWith("LENGTH")) l_length = list.at(1).toDouble(&ok);           // getting the length
        else if(list.at(0).startsWith("ANGLE")) l_angle = list.at(1).toDouble(&ok);             // getting the angle
        else if(list.at(0).startsWith("TEMPERATURE")) l_temperature = list.at(1).toDouble(&ok); // getting the temp
        else if(list.at(0).startsWith("GASGRAVITY")) l_sg_gas = list.at(1).toDouble(&ok);       // getting the gas sg
        else if(list.at(0).startsWith("OILDENSITY")) l_den_oil = list.at(1).toDouble(&ok);      // getting the oil den
        else if(list.at(0).startsWith("WATERDENSITY")) l_den_wat = list.at(1).toDouble(&ok);    // getting the water den
        else if(list.at(0).startsWith("OILVISCOSITY")) l_vis_oil = list.at(1).toDouble(&ok);    // getting the oil visc
        else if(list.at(0).startsWith("WATERVISCOSITY")) l_vis_wat = list.at(1).toDouble(&ok);  // getting the water visc
        else
        {
            if(StringUtilities::isEmpty(list))
            {
                QString message = QString("File: %s\nKeyword:%s\nNot understood in current context. Expecting a property.")
                        .arg(file.fileName().toLatin1().constData()).arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNKNOWN_KEYWORD, message);
            }
        }
        if(!ok) break;
        list = StringUtilities::processLine(file.readLine());
    }
    if(!ok)
    {
        QString message = QString("File: %s\nDefinition is incomplete.\nLast line: %s")
                .arg(file.fileName().toLatin1().constData()).arg(list.join(" ").toLatin1().constData());
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNKNOWN_KEYWORD, message);
    }
    // setting the parameters
    bb->setAngle(l_angle);
    bb->setDiameter(l_diameter);
    bb->setGasSpecificGravity(l_sg_gas);
    bb->setLength(l_length);
    bb->setOilDensity(l_den_oil);
    bb->setOilViscosity(l_vis_oil);
    bb->setTemperature(l_temperature);
    bb->setWaterDensity(l_den_wat);
    bb->setWaterViscosity(l_vis_wat);
    return bb;
}

PressureDropCalculator *PipeReader::readFile(const QString &file_name)
{
    PressureDropCalculator *calc = 0;

    QFile input(file_name);
    if(!input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString message = QString("Could not open pipe input file: %s").arg(file_name.toLatin1().constData());
        emitException(ExceptionSeverity::ERROR, ExceptionType::FILE_NOT_FOUND, message);
    }

    // starting to read the file, finding what type of calculator to produce

    QStringList list = StringUtilities::processLine(input.readLine());
    while(!input.atEnd() && !list.at(0).startsWith("EOF"))
    {
        if(list.at(0).startsWith("CORRELATION"))
        {
            if(list.at(1).startsWith("BB73")) calc = readBeggsBrill(input);
        }
        else if(StringUtilities::isEmpty(list))
        {
            QString message = QString("File: %s\nKeyword:%s\nNot understood in current context. Expecting a calculator definition.")
                    .arg(file_name.toLatin1().constData()).arg(list.join(" ").toLatin1().constData());
            emitException(ExceptionSeverity::ERROR, ExceptionType::UNKNOWN_KEYWORD, message);
        }
        list = StringUtilities::processLine(input.readLine());
    }
    input.close();
    return calc;
}
