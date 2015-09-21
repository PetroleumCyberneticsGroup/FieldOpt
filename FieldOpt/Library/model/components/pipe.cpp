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

#include "pipe.h"

Pipe::Pipe(const Pipe &p)
    : Component(p)
{
    m_number = p.m_number;
    m_file_name = p.m_file_name;
    m_schedule = p.m_schedule;
    if (p_calculator != 0)
        p_calculator = p.p_calculator->clone();
    else
        p_calculator = 0;
}

Pipe::~Pipe()
{
    if (p_calculator != 0) delete p_calculator;
}

void Pipe::emptyStreams()
{
    for (int i = 0; i < numberOfStreams(); ++i) {
        stream(i)->setGasRate(0);
        stream(i)->setOilRate(0);
        stream(i)->setWaterRate(0);
        stream(i)->setPressure(0);
    }
}

void Pipe::initialize(const QVector<double> &schedule)
{
    m_schedule = schedule;
    clearStreams();
    for (int i = 0; i < m_schedule.size(); ++i) {
        addStream(new Stream(schedule.at(i), 0, 0, 0, 0));
    }
}

void Pipe::calculateBranchInletPressures()
{
    calculateInletPressure(); // Calculate inlet pressure for the top node
    for (int i = 0; i < m_feed_pipes.size(); ++i) {
        m_feed_pipes.at(i)->calculateBranchInletPressures(); // Calculate inlet pressure for sub-branches.
    }
}

void Pipe::cleanFeedConnections()
{
    m_feed_pipes.clear();
    m_feed_wells.clear();
}

void Pipe::readInputFile()
{
    PipeReader reader;
    p_calculator = reader.readFile(m_file_name);
}

bool Pipe::addToStream(int i, const Stream &s)
{
    if (i >= numberOfStreams() || i < 0) { // Attempting to access an out of bounds Stream
        QString message = QString("Trying to add to a stream that does not exist. Exiting. \n PIPE: %1 \n STREAM: %2\n").arg(number()).arg(i);
        emitException(ExceptionSeverity::ERROR, ExceptionType::OUT_OF_BOUNDS, message);
    }
    *stream(i) += s;
    return true;
}
