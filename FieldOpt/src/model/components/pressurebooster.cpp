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

#include "pressurebooster.h"

PressureBooster::PressureBooster() :
    p_cost(0),
    p_outlet_connection(0)
{}

PressureBooster::PressureBooster(const PressureBooster &p) :
    Pipe(p)
{
    p_cost = new Cost(*p.p_cost);
    p_outlet_connection = new PipeConnection(*p.p_outlet_connection);
    p_install_time = shared_ptr<IntVariable>(new IntVariable(*p.p_install_time));
    p_pressure_change = shared_ptr<RealVariable>(new RealVariable(*p.p_pressure_change));
    p_capacity = shared_ptr<RealVariable>(new RealVariable(*p.p_capacity));

    for(int i = 0; i < p.m_capacity_constraints.size(); ++i)
        m_capacity_constraints.push_back(shared_ptr<Constraint>(new Constraint(*p.m_capacity_constraints.at(i))));
}

PressureBooster::~PressureBooster()
{
    if(p_cost != 0)
        delete p_cost;
    if(p_outlet_connection != 0)
        delete p_outlet_connection;
}

void PressureBooster::calculateInletPressure()
{
    // checking if the outlet connection is defined
    if(p_outlet_connection == 0)
    {
        QString message = QString("No outlet pipe set for booster #%1").arg(number());
        emitException(ExceptionSeverity::ERROR, ExceptionType::MODEL_VALIDATION, message);
    }

    // checking if outlet pipe have the same number of streams
    if(numberOfStreams() != outletConnection()->pipe()->numberOfStreams())
    {
        QString message = QString("Booster and upstream pipe do not have the same number of time steps.\nBooster #%1 n=%2\nUpstream pipe: #%3 n=%4")
                .arg(number()).arg(numberOfStreams()).arg(outletConnection()->pipe()->number()).arg(outletConnection()->pipe()->numberOfStreams());
        emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
    }

    // looping through the time steps
    for(int i = 0; i < numberOfStreams(); i++)
    {
        // getting the outlet pressure
        double p_out = outletConnection()->pipe()->stream(i)->pressure(stream(i)->inputUnits());

        // if the booster is installed, the inlet pressure should be outlet - pressure_change
        // else the inlet pressure should be equal to the outlet pressure
        double p_in = p_out;
        if(p_install_time->value() <= i)
        {
            p_in -= p_pressure_change->value();
            if(p_in < 0.1)
            {
                QString message = QString("The downstream pressure for booster %1 is less than the boosting bressure. \n Setting the upstream pressure to 0.").arg(number());
                emitException(ExceptionSeverity::WARNING, ExceptionType::INCONSISTENT, message);
                p_in = 0.1;
            }
        }
        stream(i)->setPressure(p_in);
    }
}

QString PressureBooster::description() const
{
    QString str("START BOOSTER\n");
    str.append(" NUMBER " + QString::number(number()) + "\n");
    if(p_install_time != 0)
        str.append(" INSTALLTIME " + QString::number(p_install_time->value()) + " " + QString::number(p_install_time->max()) + " " + QString::number(p_install_time->min()) + "\n");
    str.append(" COST " + QString::number(p_cost->constantCost()) + " " + QString::number(p_cost->fractionCost())  + "\n");
    str.append(" OUTLETPIPE " + QString::number(p_outlet_connection->pipeNumber()) + "\n");
    str.append(" PRESSUREBOOST " + QString::number(p_pressure_change->value()) + " " + QString::number(p_pressure_change->max()) + " " + QString::number(p_pressure_change->min()) + "\n");
    str.append(" CAPACITY " + QString::number(p_capacity->value()) + " " + QString::number(p_capacity->max()) + " " + QString::number(p_capacity->min()) + "\n");
    str.append("END BOOSTER\n\n");
    return str;
}

void PressureBooster::setupCapacityConstraints(const QVector<double> &master_schedule)
{
    m_capacity_constraints.clear();

    for(int i = 0; i < master_schedule.size(); ++i)
    {
        shared_ptr<Constraint> c(new Constraint(0.5, 1.0, 0.0));
        c->setName("Capacity constraint for BOOSTER #" + QString::number(number()) + " for time = " + QString::number(master_schedule.at(i)));
        m_capacity_constraints.push_back(c);
    }
}

void PressureBooster::updateCapacityConstraints()
{
    // checking that the number of streams correspond to the number of constraints
    if(numberOfStreams() != m_capacity_constraints.size())
    {
        QString message = QString("Number of streams and number of capacity constraints do not match for Booster %1.\nStreams: %2\nConstraints: %3")
                .arg(number()).arg(numberOfStreams()).arg(m_capacity_constraints.size());
        emitException(ExceptionSeverity::ERROR, ExceptionType::MODEL_VALIDATION, message);
    }

    // starting to update the constraint values
    for(int i = 0; i < numberOfStreams(); ++i)
    {
        // calculating a total rate for the time step
        double q_tot = stream(i)->oilRate(true) + stream(i)->waterRate(true) + stream(i)->gasRate(true);

        // calculating the constraint value
        double c = (p_capacity->value() - q_tot) / p_capacity->value();
        if(c > 1.0) c = 1.0;

        // updating the constraint value
        if(p_install_time->value() > i) m_capacity_constraints.at(i)->setValue(0.5);
        else m_capacity_constraints.at(i)->setValue(c);
    }
}

