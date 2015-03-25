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

#include "simulationresults.h"

SimulationResults::SimulationResults(int id, float fopt)
{
    model_id = id;
    model_fopt = fopt;
}

float SimulationResults::getModel_fopt() const
{
    return model_fopt;
}

void SimulationResults::setModel_fopt(float value)
{
    model_fopt = value;
}

int SimulationResults::getModel_id() const
{
    return model_id;
}

void SimulationResults::setModel_id(int value)
{
    model_id = value;
}

QString SimulationResults::toString() const
{
    QString mid = QString("Model id: %1").arg(model_id);

    QString fopt = QString("FOPT: %1").arg(model_fopt);
    return mid + "\n" + fopt;
}
