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

#include "well.h"

Well::Well() :
    m_bhp_inj(WellControl::QWAT),
    p_cost(0),
    p_well_path(0)
{}

Well::Well(const Well &w) :
    Component(w),
    p_cost(0),
    p_well_path(0)
{
    m_type = w.m_type;
    m_name = w.m_name;
    m_group = w.m_group;
    m_bhp_limit = w.m_bhp_limit;
    m_bhp_inj = w.m_bhp_inj;

    for(int i = 0; i < w.m_connections.size(); i++)
        m_connections.push_back(new WellConnection(*w.m_connections.at(i)));

    for(int i = 0; i < w.m_var_connections.size(); ++i)
        m_var_connections.push_back(new WellConnectionVariable(*w.m_var_connections.at(i)));

    for(int i = 0; i < w.numberOfControls(); i++)
        m_schedule.push_back(new WellControl(*w.m_schedule.at(i)));

    if(w.p_cost != 0)
        p_cost = new Cost(*w.p_cost);
    if(w.p_install_time != 0)
        p_install_time = shared_ptr<IntVariable>(new IntVariable(*w.p_install_time));
    if(w.p_well_path != 0)
        p_well_path = new WellPath(*w.p_well_path);
}

Well::~Well()
{
    for(int i = 0; i < m_connections.size(); i++)
        delete m_connections.at(i);

    for(int i = 0; i < m_var_connections.size(); ++i)
        delete m_var_connections.at(i);

    for(int i = 0; i < m_schedule.size(); i++)
        delete m_schedule.at(i);

    if(p_cost != 0)
        delete p_cost;
    if(p_well_path != 0)
        delete p_well_path;
}

void Well::initialize()
{
    clearStreams();
    for(int i = 0; i < m_schedule.size(); ++i)
        addStream(new Stream(m_schedule.at(i)->endTime(), 0, 0, 0, 0));
}

bool Well::isInstalled(int i)
{
    if(p_install_time == 0)
        return true;
    else
        return (i >= p_install_time->value());
}

void Well::addVariableConnection(WellConnectionVariable *vc)
{
    vc->iVariable()->setName("I-direction connection variable for well: " + name());
    vc->jVariable()->setName("J-direction connection variable for well: " + name());
    m_var_connections.push_back(vc);
}

WellConnection *Well::connection(int i) const
{
    if(i < m_connections.size())
        return m_connections.at(i);
    else if(i < (m_connections.size() + m_var_connections.size()))
        return m_var_connections.at(i - m_connections.size())->wellConnection();
    else return 0;
}
