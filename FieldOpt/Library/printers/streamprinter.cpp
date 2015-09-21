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

#include "streamprinter.h"

void StreamPrinter::printStream(const Stream& stream) const
{
    QString l1 = "Stream data:";
    QString l2 = QString("TIME\t\t= %1").arg(stream.time());
    QString l3 = QString("GAS RATE\t= %1").arg(stream.gasRate());
    QString l4 = QString("OIL RATE\t= %1").arg(stream.oilRate());
    QString l5 = QString("WATER RATE\t= %1").arg(stream.waterRate());
    QString l6 = QString("PRESSURE\t= %1").arg(stream.pressure());
    QString l7 = stream.inputUnits() == Stream::METRIC ? "UNITS\t\t= METRIC" : "UNITS\t\t= FIELD";
    QString message = QString("%1\n%2\n%3\n%4\n%5\n%6\n%7").arg(l1).arg(l2).arg(l3).arg(l4).arg(l5).arg(l6).arg(l7);
    printMessage(message);
}
