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

#include "injectionwell.h"



QString InjectionWell::description() const
{
    QString str("START WELL\n");
    str.append(" NAME " + name() + "\n");
    str.append(" TYPE I \n");
    str.append(" GROUP " + group() + "\n");
    str.append(" BHPLIMIT " + QString::number(bhpLimit()) + "\n");
    str.append(" BHPINJ ");
    if(bhpInj() == WellControl::QWAT)
        str.append("WATER\n");
    else if(bhpInj() == WellControl::QOIL)
        str.append("OIL\n");
    else if(bhpInj() == WellControl::QGAS)
        str.append("GAS\n");

    if(hasInstallTime())
        str.append(" INSTALLTIME " + QString::number(installTime()->value()) + " " + QString::number(installTime()->max()) + " " + QString::number(installTime()->min()) + "\n");

    if(hasCost())
        str.append(" COST " + QString::number(cost()->constantCost()) + " " + QString::number(cost()->fractionCost()) + " " + QString::number(cost()->capacityCost()) + "\n");

    if(numberOfConstantConnections() > 0) {
        str.append(" START CONNECTIONS\n");
        for(int i = 0; i < numberOfConnections(); ++i)
        {
            WellConnection *wc = constantConnection(i);
            if(wc->cell() >= 0)
                str.append("  " + QString::number(wc->cell()) + " " + QString::number(wc->wellIndex()) + "\n");
            else str.append("  " + QString::number(wc->i()) + " " +
                            QString::number(wc->j()) + " " +
                            QString::number(wc->k1()) + " " +
                            QString::number(wc->k2()) + " " +
                            QString::number(wc->wellIndex()) + "\n");
        }
        str.append(" END CONNECTIONS\n\n");
    }

    if(numberOfVariableConnections() > 0) {
        str.append(" START VARCONNECTIONS\n");
        for(int i = 0; i < numberOfVariableConnections(); ++i)
        {
            WellConnectionVariable *wcv = variableConnection(i);
            str.append("  " + QString::number(wcv->iVariable()->value()) + " " +
                       QString::number(wcv->iVariable()->max()) + " " +
                       QString::number(wcv->iVariable()->min()) + " " +
                       QString::number(wcv->jVariable()->value()) + " " +
                       QString::number(wcv->jVariable()->max()) + " " +
                       QString::number(wcv->jVariable()->min()) + " " +
                       QString::number(wcv->wellConnection()->k1()) + " " +
                       QString::number(wcv->wellConnection()->k2()) + " " +
                       QString::number(wcv->wellConnection()->wellIndex()) + "\n");
        }
        str.append(" END VARCONNECTIONS\n");
    }
    str.append(" START SCHEDULE\n");
    for(int i = 0; i < numberOfControls(); ++i)
        str.append(control(i)->description());

    str.append(" END SCHEDULE\n\n");
    str.append("END WELL\n\n");
    return str;
}
