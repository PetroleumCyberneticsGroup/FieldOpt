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

#include "npvobjective.h"

NpvObjective::NpvObjective() :
    m_dcf(0.0),
    m_price_oil(0.0),
    m_price_gas(0.0),
    m_price_water(0.0)
{}

QString NpvObjective::description() const
{
    QString str("START OBJECTIVE\n");
    str.append(" TYPE NPV \n\n");

    str.append(" DCF " + QString::number(dcf()) + "\n");
    str.append(" OILPRICE " + QString::number(oilPrice()) + "\n");
    str.append(" GASPRICE " + QString::number(gasPrice()) + "\n");
    str.append(" WATERPRICE " + QString::number(waterPrice()) + "\n");
    str.append("END OBJECTIVE\n\n");
    return str;
}

void NpvObjective::calculateValue(QVector<Stream *> s, QVector<Cost *> c)
{
    // checking if the discount factor is entered as fraction or percent
    if(m_dcf >= 1.0)
    {
        QString message = QString("The discount factor was entered as: %1. Assuming this is a percentage.").arg(m_dcf);
        emitException(ExceptionSeverity::WARNING, ExceptionType::ASSUMPTION, message);
        m_dcf = m_dcf / 100.0;
    }

    double npv = 0;
    int cost_place = 0;
    bool add_more_costs = true;

    // first adding the up front costs (costs where t < t0)
    while(cost_place < c.size() && add_more_costs)
    {
        if(c.at(cost_place)->time() < s.at(0)->time())
        {
            npv -= c.at(cost_place)->value();
            ++cost_place;
        }
        else
            add_more_costs = false;
    }
    for(int i = 0; i < s.size(); i++)       // looping through each time step
    {
        double dt;            // time step duration
        double cf = 0.0;      // cash flow over time step

        // calculate time step duration
        if(i == 0)
            dt = s.at(i)->time();
        else
            dt = s.at(i)->time() - s.at(i-1)->time();

        // calculate cash flow
        cf = dt * (s.at(i)->gasRate(s.at(i)->inputUnits()) * gasPrice());        // gas
        cf += dt * (s.at(i)->oilRate(s.at(i)->inputUnits()) * oilPrice());       // oil
        cf += dt * (s.at(i)->waterRate(s.at(i)->inputUnits()) * waterPrice());   // water

        // adding costs that fall within this time step
        double ts_cost = 0;
        add_more_costs = true;
        while(cost_place < c.size() && add_more_costs)
        {
            if(c.at(cost_place)->time() <= s.at(i)->time())
            {
                ts_cost += c.at(cost_place)->value();
                ++cost_place;
            }
            else
                add_more_costs = false;
        }
        // subtracting the costs from the cash flow
        cf -= ts_cost;
        // add and discount cash flow to NPV
        double time_yr = s.at(i)->time() / 365;
        npv += cf / pow(1+dcf(), time_yr);
    }
    setValue(npv);
}
