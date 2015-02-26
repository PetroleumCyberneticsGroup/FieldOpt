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

#ifndef READER_H
#define READER_H

#include <QObject>
#include "exceptionhandler/readerhandler.h"

/*!
 * \brief Reader is the parent class of all file readers. It primarily deals with handling exceptions and printing warnings/errors to the console.
 */
class Reader : public QObject
{
    Q_OBJECT
public:
    explicit Reader(QObject *parent = 0);
    ~Reader();

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

protected:
    /*!
     * \brief Convenience class used by the component subclasses to emit exceptions.
     * \param severity The severity of the event.
     * \param type The type of exception.
     * \param message The message to be printed.
     */
    void emitException(ExceptionSeverity severity, ExceptionType type, QString message);
};

#endif // READER_H
