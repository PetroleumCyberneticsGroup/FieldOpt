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

#include "caseprinter.h"

void CasePrinter::printCase(const Case &c) const
{
    QString l1 = "Case data:";
    QString l2 = QString("OBJ\t\t= %1").arg(c.objectiveValue());

    QString l3 = "";
    for (int i = 0; i < c.numberOfRealVariables(); ++i) {
        l3.append(QString("VAR_C_%1 = %2\n").arg(i+1).arg(c.realVariableValue(i)));
    }

    QString l4 = "";
    for (int i = 0; i < c.numberOfBinaryVariables(); ++i) {
        l4.append(QString("VAR_B_%1 = %2\n").arg(i+1).arg(c.binaryVariableValue(i)));
    }

    QString l5 = "";
    for (int i = 0; i < c.numberOfIntegerVariables(); ++i) {
        l5.append(QString("VAR_I_%1 = %2\n").arg(i+1).arg(c.integerVariableValue(i)));
    }

    QString l5 = "";
    for (int i = 0; i < c.numberOfConstraints(); ++i) {
        l5.append(QString("CON_%1 = %2\n").arg(i+1).arg(c.constraintValue(i)));
    }

     QString l6 = "";
    if (c.p_objective_derivative != 0) {
        for (int i = 0; i < c.objectiveDerivative()->numberOfPartials(); ++i) {
            l6.append(QString("OBJECTIVE DERIVATIVES:\n%1 = %2\n").arg(1).arg(c.objectiveDerivative()->value(i)));
        }
    }

    QString message = QString("%1\n%2\n%3\n%4\n%5\n%6").arg(l1).arg(l2).arg(l3).arg(l4).arg(l5).arg(l6);
    printMessage(message);
}
