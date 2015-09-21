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

#ifndef CAPACITY_H
#define CAPACITY_H

#include <QVector>
#include <QString>
#include <QObject>
#include <tr1/memory>

#include "model/components/pipe.h"
#include "constraint.h"
#include "exceptionhandler/constrainthandler.h"

using std::tr1::shared_ptr;

class Constraint;
class Pipe;

/*!
 * \brief Capacity constraints a given point in the pipe network.
 *
 * A capacity constraint is used to set maximum allowed rates for the different phases at points in the pipe network.
 * Constraints can be set for the maximum rate of gas, oil, water, or total liquid (oil + water).
 * This is done through the setMax() functions.
 *
 * The Capacity is fed by any number of pipes.
 * When the Capacity is generated from the driver file, the identifying pipe numbers are added through addFeedPipeNumber().
 * The Model takes care of translating the numbers to Pipe pointers during initialization.
 * The pointers to the feed pipes are added through addFeedPipe().
 *
 * \todo Could likely be rewritten to use the Capacity parent class.
 */
class Capacity : QObject
{
    Q_OBJECT

signals:
    /*!
     * \brief Error signal.
     *
     * Emitted when an error occurs.
     *
     * \param severity The severity of the event. May be WARNING or ERROR.
     * \param type The type of exception.
     * \param message The message to be printed in addition to the severity and the type.
     */
    void error(ExceptionSeverity severity, ExceptionType type, const QString message);

private:
    QString m_name;                                  //!< Name of this constraint.
    double m_max_oil;                                //!< Max oil constraint.
    double m_max_gas;                                //!< Max gas constraint.
    double m_max_water;                              //!< Max water constraint.
    double m_max_liquid;                             //!< Max liquid constraint.
    QVector<shared_ptr<Constraint> > m_cons_oil;     //!< Oil constraints
    QVector<shared_ptr<Constraint> > m_cons_gas;     //!< Gas constraints
    QVector<shared_ptr<Constraint> > m_cons_water;   //!< Water constraints
    QVector<shared_ptr<Constraint> > m_cons_liquid;  //!< Liquid constraints
    QVector<Pipe*> m_feed_pipes;                     //!< \todo This is marked as todo in the ResOpt code.
    QVector<int> m_feed_pipe_numbers;                //!< Feed pipe numbers (as input in the driver file)
    QVector<double> m_schedule;                      //!< The schedule.

public:
    Capacity();                   //!< Default constructor. Sets max values to -1.0.
    Capacity(const Capacity &s);  //!< Copy constuctor. Does not copy the feed pipes, as they need to be resolved from the model.

    void setupConstraints(const QVector<double> &master_schedule);
    void updateConstraints();  //!< Update the constraints based on the maximum rates going through the separator
    QString description() const;  //!< Get a description for the driver file.

    void addFeedPipe(Pipe *p) {m_feed_pipes.push_back(p);}  //!< Add a Pipe as a feed to the Separator
    void addFeedPipeNumber(int i) {m_feed_pipe_numbers.push_back(i);}  //!< Add an identifying number to a feed pipe. The numbers must be resolved to pointers before running the model.

    void setName(const QString &n) {m_name = n;}  //!< Set the separator name
    void setMaxOil(double m) {m_max_oil = m;}  //!< Set the oil Constraint for the Separator
    void setMaxGas(double m) {m_max_gas = m;}  //!< Set the gas Constraint for the Separator
    void setMaxWater(double m) {m_max_water = m;}  //!< Set the water Constraint for the Separator
    void setMaxLiquid(double m) {m_max_liquid = m;}  //!< Set the total liquid Constraint for the Separator

    double maxOil() const {return m_max_oil;}                                     //!< Get max oil
    double maxGas() const {return m_max_gas;}                                     //!< Get max gas
    double maxWater() const {return m_max_water;}                                 //!< Get max water
    double maxLiquid() const {return m_max_liquid;}                               //!< Get max liquid
    const QVector<double>& schedule() const {return m_schedule;}                  //!< Get the schedule.
    const QString& name() const {return m_name;}                                  //!< Get the separator name
    QVector<shared_ptr<Constraint> > oilConstraints() {return m_cons_oil;}        //!< Get the oil Constraint for the Separator
    QVector<shared_ptr<Constraint> > gasConstraints() {return m_cons_gas;}        //!< Get the gas Constraint for the Separator
    QVector<shared_ptr<Constraint> > waterConstraints() {return m_cons_water;}    //!< Get Returns the water Constraint for the Separator
    QVector<shared_ptr<Constraint> > liquidConstraints() {return m_cons_liquid;}  //!< Returns the total liquid Constraint for the Separator
    int numberOfFeedPipeNumbers() const {return m_feed_pipe_numbers.size();}      //!< The number of feed pipes specified in the driver file. __Does not neccesary correspond to the number of connected pipes.__
    int feedPipeNumber(int i) const {return m_feed_pipe_numbers.at(i);}           //!< Get the number of feed pipe i (as input in the driver file)
    int numberOfFeedPipes() const {return m_feed_pipes.size();}                   //!< Get the total number of feed pipes.
    Pipe* feedPipe(int i) {return m_feed_pipes.at(i);}                            //!< Get feed pipe i.

protected:
    /*!
     * \brief Convenience class used by the component subclasses to emit exceptions.
     * \param severity The severity of the event.
     * \param type The type of exception.
     * \param message The message to be printed.
     */
    void emitException(ExceptionSeverity severity, ExceptionType type, QString message);
};

#endif // CAPACITY_H
