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

#ifndef STREAM_H
#define STREAM_H

#include <QVector>
#include <QObject>
#include "printers/streamprinter.h"

/*!
 * \brief Container for rates at a given time step
 */
class Stream : public QObject
{
    Q_OBJECT
public:
    enum units{ METRIC, FIELD };

private:
    double m_time;         //!< The current time.
    double m_oil_rate;     //!< Oil rate at current time.
    double m_water_rate;   //!< Water rate at current time.
    double m_gas_rate;     //!< Gas rate at current time.
    double m_pressure;     //!< Pressure in the stream at current time.
    units m_input_units;   //!< The units for the values in this stream. May be either METRIC or FIELD.

signals:
    void printStream(const Stream& stream) const; //!< Received by the StreamPrinter class. Print a representation of the Stream to the console.

public:
    /*!
     * \brief Initializes all rates and pressure to 0.0. Sets units to FIELD.
     */
    Stream();

    /*!
     * \brief Initializes values to the parameter values. Sets units to FIELD.
     * \param time Current time.
     * \param oil_rate Oil rate.
     * \param gas_rate Gas rate.
     * \param water_rate Water rate.
     * \param pressure Pressure in the stream.
     */
    Stream(double time, double oil_rate, double gas_rate, double water_rate, double pressure);

    /*!
     * \brief Copy constructor. Copies the fields from the parameter Stream.
     * \param s Stream to be copied.
     */
    Stream(const Stream &s);

    /*!
     * \brief Set the fields in this Stream to the average of the values of the Streams in the parameter Stream list.
     *
     * The values are calculated by first calculating the cumulative production by multiplying the production rate in each
     * stream with the difference between the time set for it and the t_start parameter.
     * The cumulative production is then divided by the total total time span, which is taken to be the difference between
     * the time set for the last element in the Stream vector and the t_start parameter, to calculate the average rates.
     *
     * \param streams List of streams.
     * \param t_start Start time.
     */
    void avg(const QVector<Stream*> &streams, double t_start);

    void printToCout() const; //!< Print a representation of this stream to the console using the StreamPrinter class.

    void setTime(double time) { m_time = time; }                         //!< Set function for m_time.
    void setOilRate(double oil_rate) { m_oil_rate = oil_rate; }          //!< Set function for m_oil_rate.
    void setGasRate(double gas_rate) {m_gas_rate = gas_rate; }           //!< Set function for m_gas_rate.
    void setWaterRate(double water_rate) { m_water_rate = water_rate; }  //!< Set function for m_water_rate.
    void setPressure(double pressure) {m_pressure = pressure; }          //!< Set function for m_presssure.
    void setInputUnits(Stream::units units) { m_input_units = units; }   //!< Set function for m_input_units.

    double time() const { return m_time; }             //!< Return m_time in days
    double oilRate() const { return m_oil_rate; }      //!< Return m_oil_rate using the currently set units.
    double oilRate(Stream::units units) const;         //!< Return m_oil_rate using the parameter units.
    double gasRate() const { return m_gas_rate; }      //!< Return m_gas_rate using the currently set units.
    double gasRate(Stream::units units) const;         //!< Return m_gas_rate using the parameter units.
    double waterRate() const { return m_water_rate; }  //!< Return m_water_rate using the currently set units.
    double waterRate(Stream::units units) const;       //!< Return m_water_rate using the parameter units.
    double pressure() const { return m_pressure; }     //!< Return pressure using the currently set units.
    double pressure(Stream::units units) const;        //!< Return pressure using the parameter units.
    Stream::units inputUnits() const { return m_input_units; }  //!< Get function for m_input_units.

    /*!
     * \brief Assignment operator.
     *
     * Sets left-hand-side Stream equal to right-hand-side Stream.
     *
     * \param rhs Stream to set lhs equal to.
     * \return Copy of rhs.
     */
    Stream& operator =(const Stream &rhs);

    /*!
     * \brief Compound assignment operator.
     *
     * Sets rates equal to lhs rates plus rhs rates.
     * Sets m_time equal to rhs time; m_pressure equal to 0; m_input_units equal to rhs units.
     *
     * \param rhs Stream to be added to this stream.
     * \return Stream where the rates are equal to the sum of the lhs and the rhs Streams.
     */
    Stream& operator +=(const Stream &rhs);           //!< Compound assignment operator. Sets

    /*!
     * \brief Plus operator.
     *
     * Sets the result equal to the lsh Stream and uses the compound assignment operator to
     * add lsh and rhs.
     *
     * \param rhs
     * \return Left-hand-side Stream plus right-hand-side Stream with rhs units and pressure equal to 0.
     */
    const Stream operator +(const Stream &rhs) const;

    /*!
     * \brief Multiplication operator.
     *
     * Multiplies the rates with a double value.
     *
     * \param rhs A double to multiply the rates with.
     * \return Stream where the rates have been multiplied with a double.
     */
    const Stream operator *(const double &rhs) const;

};

#endif // STREAM_H
