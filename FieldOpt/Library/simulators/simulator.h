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

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "model/model.h"
#include "exceptionhandler/simulatorhandler.h"
#include <QString>
#include <QObject>

/*!
 * \brief Abstract parent class for all simulators.
 *
 * \todo This is a stub.
 */
class Simulator : public QObject
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
    QString m_folder;
public:
    Simulator(Model* model) {}      //!< \todo Should maybe not exist
    Simulator();                   //!< Default constructor. Does nothing.
    Simulator(const Simulator &s);  //!< Copy constructor.
    virtual ~Simulator(){}          //!< Default constructor.

    virtual Simulator* clone() const = 0;  //!< Get a copy of this simulator.
    virtual QString description() const = 0;       //!< Get a description of this simulator.

    virtual bool generateInputFiles(Model *m) = 0;  //!< Generate the input files the specific simulator needs.
    virtual bool launchSimulator() = 0;             //!< Launch the simulator. \todo make virtual
    virtual bool readOutput(Model *m) = 0;          //!< Read the output files. \todo make virtual

    const QString& folder() {return m_folder;}                  //!< Get the working directory.
    void setFolder(const QString &folder) {m_folder = folder;}  //!< Set the working directory.

protected:
    /*!
     * \brief Convenience class used by the component subclasses to emit exceptions.
     * \param severity The severity of the event.
     * \param type The type of exception.
     * \param message The message to be printed.
     */
    void emitException(ExceptionSeverity severity, ExceptionType type, QString message);
    void emitProgress(QString message);
};

#endif // SIMULATOR_H
