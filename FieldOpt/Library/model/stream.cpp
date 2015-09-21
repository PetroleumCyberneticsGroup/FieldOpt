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

#include "stream.h"

Stream::Stream()
{
    m_time = 0;
    m_oil_rate = 0.0;
    m_gas_rate = 0.0;
    m_water_rate = 0.0;
    m_pressure = 0.0;
    m_input_units = Stream::FIELD;
}

Stream::Stream(double time, double oil_rate, double gas_rate, double water_rate, double pressure)
{
    m_time = time;
    m_oil_rate = oil_rate;
    m_gas_rate = gas_rate;
    m_water_rate = water_rate;
    m_pressure = pressure;
    m_input_units = Stream::FIELD;
}

Stream::Stream(const Stream &s)
{
    m_time = s.m_time;
    m_oil_rate = s.m_oil_rate;
    m_gas_rate = s.m_gas_rate;
    m_water_rate= s.m_water_rate;
    m_pressure = s.m_pressure;
    m_input_units = s.m_input_units;
}

void Stream::avg(const QVector<Stream *> &streams, double t_start)
{
    // Set time equal to time of the last stream in the vector.
    m_time = streams.last()->time();

    // Calculate cumulative production
    double cum_oil = 0.0;
    double cum_gas = 0.0;
    double cum_wat = 0.0;
    double cum_pre = 0.0;
    double t = t_start;
    for (int i = 0; i < streams.size(); ++i) {
        double dt = streams.at(i)->time() - t;
        cum_oil += streams.at(i)->oilRate() * dt;
        cum_gas += streams.at(i)->gasRate() * dt;
        cum_wat += streams.at(i)->waterRate() * dt;
        cum_pre += streams.at(i)->pressure() * dt;
        t = streams.at(i)->time();
    }

    // Setting the average reates to this stream
    double time_span = m_time - t_start;
    m_oil_rate = cum_oil / time_span;
    m_gas_rate = cum_gas / time_span;
    m_water_rate = cum_wat / time_span;
    m_pressure = cum_pre / time_span;
}

void Stream::printToCout() const
{
    StreamPrinter* sp = new StreamPrinter;
    connect(this, SIGNAL(printStream(const Stream&)), sp, SLOT(printStream(const Stream&)));
    emit printStream(*this);
    disconnect(this, SIGNAL(printStream(const Stream&)), sp, SLOT(printStream(const Stream&)));
}

double Stream::oilRate(Stream::units units) const
{
    if (m_input_units != units)
    {
        if (units == Stream::METRIC)
            return m_oil_rate * 0.158987295;
        else
            return m_oil_rate / 0.158987295;
    }
    else
        return m_oil_rate;

}

double Stream::gasRate(Stream::units units) const
{
    if (m_input_units != units)
    {
        if (units == Stream::METRIC)
            return m_gas_rate * 28.3168466;
        else
            return m_gas_rate / 28.3168466;
    }
    else
        return m_gas_rate;
}

double Stream::waterRate(Stream::units units) const
{
    if (m_input_units != units)
    {
        if (units == Stream::METRIC)
            return m_water_rate * 0.158987295;
        else
            return m_water_rate / 0.158987295;
    }
    else
        return m_water_rate;
}

double Stream::pressure(Stream::units units) const
{
    if (m_input_units != units)
    {
        if (units == Stream::METRIC)
            return m_pressure / 14.5037738;
        else
            return m_pressure * 14.5037738;
    }
    else
        return m_pressure;
}

Stream &Stream::operator =(const Stream &rhs)
{
    if (this != &rhs)
    {
        setTime(rhs.time());
        setOilRate(rhs.oilRate());
        setGasRate(rhs.gasRate());
        setWaterRate(rhs.waterRate());
        setPressure(rhs.pressure());
        setInputUnits(rhs.inputUnits());
    }
    return *this;
}

Stream &Stream::operator +=(const Stream &rhs)
{
    setTime(rhs.time());
    setPressure(0.0);
    setOilRate(oilRate(rhs.inputUnits()) + rhs.oilRate());
    setGasRate(gasRate(rhs.inputUnits()) + rhs.gasRate());
    setWaterRate(waterRate(rhs.inputUnits()) + rhs.waterRate());
    setInputUnits(rhs.inputUnits());
    return *this;
}

const Stream Stream::operator +(const Stream &rhs) const
{
    Stream result = *this;
    result += rhs;
    return result;
}

const Stream Stream::operator *(const double &rhs) const
{
    Stream result = *this;
    result.setOilRate(result.oilRate() * rhs);
    result.setGasRate(result.gasRate() * rhs);
    result.setWaterRate(result.waterRate() * rhs);
    return result;
}
