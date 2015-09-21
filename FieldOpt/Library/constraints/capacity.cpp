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

#include "capacity.h"

Capacity::Capacity() :
    m_max_oil(-1.0),
    m_max_gas(-1.0),
    m_max_water(-1.0),
    m_max_liquid(-1.0)
{}

Capacity::Capacity(const Capacity &s)
{
    m_name = s.m_name;
    m_max_oil = s.m_max_oil;
    m_max_gas = s.m_max_gas;
    m_max_water = s.m_max_water;
    m_max_liquid = s.m_max_liquid;
    m_schedule = s.m_schedule;

    for(int i = 0; i < s.numberOfFeedPipeNumbers(); i++)
        m_feed_pipe_numbers.push_back(s.feedPipeNumber(i));

    for(int i = 0; i < s.m_cons_oil.size(); ++i)
        m_cons_oil.push_back(shared_ptr<Constraint>(new Constraint(*s.m_cons_oil.at(i))));

    for(int i = 0; i < s.m_cons_gas.size(); ++i)
        m_cons_gas.push_back(shared_ptr<Constraint>(new Constraint(*s.m_cons_gas.at(i))));

    for(int i = 0; i < s.m_cons_water.size(); ++i)
        m_cons_water.push_back(shared_ptr<Constraint>(new Constraint(*s.m_cons_water.at(i))));

    for(int i = 0; i < s.m_cons_liquid.size(); ++i)
        m_cons_liquid.push_back(shared_ptr<Constraint>(new Constraint(*s.m_cons_liquid.at(i))));
}

void Capacity::setupConstraints(const QVector<double> &master_schedule)
{
    m_schedule = master_schedule;

    m_cons_oil.clear();
    m_cons_gas.clear();
    m_cons_water.clear();
    m_cons_liquid.clear();

    if(m_max_oil >= 0)
    {
        for(int i = 0; i < master_schedule.size(); ++i)
        {
            shared_ptr<Constraint> c_oil(new Constraint());
            c_oil->setMax(1.0);
            c_oil->setMin(-1.0);
            c_oil->setValue(0);
            c_oil->setName("Oil production constraint for capacity: " + name() + " for time = " + QString::number(master_schedule.at(i)));
            m_cons_oil.push_back(c_oil);
        }
    }

    if(m_max_gas >= 0)
    {
        for(int i = 0; i < master_schedule.size(); ++i)
        {
            shared_ptr<Constraint> c_gas(new Constraint());
            c_gas->setMax(1.0);
            c_gas->setMin(-1.0);
            c_gas->setValue(0);
            c_gas->setName("Gas production constraint for capacity: " + name() + " for time = " + QString::number(master_schedule.at(i)));
            m_cons_gas.push_back(c_gas);
        }
    }

    if(m_max_water >= 0)
    {
        for(int i = 0; i < master_schedule.size(); ++i)
        {
            shared_ptr<Constraint> c_wat(new Constraint());
            c_wat->setMax(1.0);
            c_wat->setMin(-1.0);
            c_wat->setValue(0);
            c_wat->setName("Water production constraint for capacity: " + name() + " for time = " + QString::number(master_schedule.at(i)));
            m_cons_water.push_back(c_wat);
        }
    }

    if(m_max_liquid >= 0)
    {
        for(int i = 0; i < master_schedule.size(); ++i)
        {
            shared_ptr<Constraint> c_liq(new Constraint());
            c_liq->setMax(1.0);
            c_liq->setMin(-1.0);
            c_liq->setValue(0);
            c_liq->setName("Liquid production constraint for capacity: " + name() + " for time = " + QString::number(master_schedule.at(i)));
            m_cons_liquid.push_back(c_liq);
        }
    }
}

void Capacity::updateConstraints()
{
    if(numberOfFeedPipes() > 0)
    {
        // check that all the feed pipes have the same number of streams as the schedule
        for(int i = 0; i < numberOfFeedPipes(); ++i)
        {
            if(feedPipe(i)->numberOfStreams() != m_schedule.size())
            {
                QString message = QString("Separator and pipe do not have the same number of time steps.\nSEP: %1\nPIPE: %2")
                        .arg(name().toLatin1().constData()).arg(feedPipe(i)->number());
                emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
            }
        }

        // looping through the time steps
        for(int i = 0; i < feedPipe(0)->numberOfStreams(); i++)
        {
            Stream s;
            for(int j = 0; j < numberOfFeedPipes(); j++)
            {
                s += *feedPipe(j)->stream(i);   // adding feed pipe rate to the separator rate
            }
            if(m_max_oil >= 0)
            {
                double scaled_oil = (s.oilRate() - m_max_oil) / m_max_oil;
                m_cons_oil.at(i)->setValue(scaled_oil + 1.0);
            }
            if(m_max_gas >= 0)
            {
                double scaled_gas = (s.gasRate() - m_max_gas) / m_max_gas;
                m_cons_gas.at(i)->setValue(scaled_gas + 1.0);
            }
            if(m_max_water >= 0)
            {
                double scaled_water = (s.waterRate() - m_max_water) / m_max_water;
                m_cons_water.at(i)->setValue(scaled_water + 1.0);
            }
            if(m_max_liquid >= 0)
            {
                double scaled_liquid = (s.oilRate() + s.waterRate() - m_max_liquid) / m_max_liquid;
                m_cons_liquid.at(i)->setValue(scaled_liquid + 1.0);
            }
        }
    }
}

QString Capacity::description() const
{
    QString str("START CAPACITY\n");
    str.append(" NAME " + name() + "\n");
    str.append(" PIPES ");
    for(int i = 0; i < numberOfFeedPipeNumbers(); ++i)
        str.append(QString::number(feedPipeNumber(i)) + " ");

    str.append("\n");

    if(m_max_gas >= 0) str.append(" GAS " + QString::number(m_max_gas) + "\n");
    if(m_max_oil >= 0) str.append(" OIL " + QString::number(m_max_oil) + "\n");
    if(m_max_water >= 0) str.append(" WATER " + QString::number(m_max_water) + "\n");
    if(m_max_liquid >= 0) str.append(" LIQUID " + QString::number(m_max_liquid) + "\n");
    str.append("END CAPACITY\n\n");
    return str;
}

void Capacity::emitException(ExceptionSeverity severity, ExceptionType type, QString message)
{
    ConstraintHandler* ch = new ConstraintHandler;
    connect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
            ch, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
    emit error(severity, type, message);
    disconnect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
               ch, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
}

