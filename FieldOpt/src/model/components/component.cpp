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

#include "component.h"

int Component::next_id = 0;

Component::Component()
{
    m_id = next_id++;
}

Component::Component(const Component& component)
{
    m_id = component.m_id;

    for (int i = 0; i < numberOfStreams(); ++i) {
        delete m_streams.at(i);
    }
    m_streams.clear();
    m_streams.resize(component.m_streams.size());

    for (int i = 0; i < component.m_streams.size(); ++i) {
        m_streams.replace(i, new Stream(*component.m_streams.at(i)));
    }
}

Component::~Component()
{
    for (int i = 0; i < numberOfStreams(); ++i) {
        delete m_streams.at(i);
    }
}

bool Component::setStream(int i, Stream *stream)
{
    if (i >= numberOfStreams() || i < 0) {
        return false;
    }
    else {
        delete m_streams.at(i);
        m_streams.replace(i, stream);
        return true;
    }
}
