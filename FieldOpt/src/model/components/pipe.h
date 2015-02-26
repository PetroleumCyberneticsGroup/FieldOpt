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

#ifndef PIPE_H
#define PIPE_H

#include "pressuredropcalculators/pressuredropcalculator.h"
#include "model/stream.h"
#include "model/components/component.h"
#include "utilities/stringutilities.h"
#include "fileio/readeres/pipereader.h"
#include <QString>
#include <QStringList>
#include <QVector>

class ProductionWell;
class PressureDropCalculator;
class Stream;
class PipeReader;

/*!
 * \brief Abstract base class for pipe segments.
 *
 * A pipe is fed by either wells or other pipes.
 * Pressure drop calculations are performed based on the input streams goung through
 * the pipe and the outlet pressure of the pipe.
 */
class Pipe : public Component
{
private:
    int m_number;                           //!< Unique identifier for the pipe.
    QString m_file_name;                    //!< The file containing the pressure drop table.
    PressureDropCalculator *p_calculator;   //!< The pressure drop calculator to be used.
    QVector<ProductionWell*> m_feed_wells;  //!< Pointers to Wells entering this pipe.
    QVector<Pipe*>  m_feed_pipes;           //!< Pointers to pipes entering this pipe.
    QVector<double> m_schedule;             //!< A copy of the master schedule set in the model. \todo Why is this necessary?

public:
    Pipe() : p_calculator(0) {}  //!< Default constructor. Initializes the calculator.
    Pipe(const Pipe &p);         //!< Copy constructor.
    ~Pipe();

    virtual Pipe* clone() = 0;                                 //!< Create a copy of the pipe.
    virtual void calculateInletPressure() = 0;                 //!< Calculate the inlet pressure of the pipe based on the outlet pressure and the incoming stream.
    virtual void emptyStreams();                               //!< Reset the rates and pressures in all the streams to zero.
    virtual void initialize(const QVector<double> &schedule);  //!< Set the schedule and populate the Stream vector in the parent Component class by adding Streams with start time from the scedule and all other values equal to 0.
    virtual QString description() const = 0;                   //!< Return a string description for the pipe.

    /*!
     * \brief Calculate the inlet pressure for all feed pipes recursively.
     *
     * The inlet pressures are calculated recursively downwards in the branch.
     * First for all the feed pipes connected to this pipe directly, then for all the feed pipes connected to the feed pipes of this pipe, and so on.
     */
    void calculateBranchInletPressure();
    void cleanFeedConnections();  //!< Remove all current feed Wells and Pipes.
    void readInputFile();         //!< Read the input file containing the pressure drop tables for this pipe segment.

    bool addToStream(int i, const Stream& s);                         //!< Add the rates in Stream s to Stream i. Return true if the stream is successfully added, otherwise exit. \todo Does not need to return anything. Rewrite to void.
    void addFeedPipe(Pipe* p) { m_feed_pipes.push_back(p); }          //!< Add Pipe p as a feed stream to this Pipe.
    void addFeedWell(ProductionWell* w) {m_feed_wells.push_back(w); } //!< Add Well w as a direct feed to this Pipe. Only Wells that feed directly to the pipe should be added here.

    void setNumber(int n) { m_number = n; }                  //!< Set the identifying number of the Pipe.
    void setFileName(const QString &s) { m_file_name = s; }  //!< Set the name of the file containing pressure drop tables for the pipe.

    int number() const { return m_number; }                        //!< Return the identifying number of the pipe.
    const QString& fileName() const { return m_file_name; }        //!< Return the name of the file containing pressure drop tables for the pipe.
    PressureDropCalculator* calculator() { return p_calculator; }  //!< Return the pressure drop calculator associated with this pipe.
};

#endif // PIPE_H
