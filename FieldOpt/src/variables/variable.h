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

#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>
#include <QObject>
#include "exceptionhandler/exceptionhandler.h"
#include "exceptionhandler/variablehandler.h"

class Variable : public QObject //!< Base class for all variables.
{
    Q_OBJECT
signals:
    /*!
     * \brief Warning signal.
     *
     * Emitted when the user does something that breaks with the model but is not critical.
     *
     * \param severity The severity of the event. May be WARNING or ERROR.
     * \param type The type of exception.
     * \param message The message to be printed in addition to the severity and the type.
     */
    void warning(ExceptionSeverity severity, ExceptionType type, const QString message);

private:
    QString m_name;        //!< Identifying name for the variable
    int m_id;              //!< Unique id number for the variable
    static int next_id;    //!< Id to be set for next variable
    //Component *p_parent;   // TODO: pointer to the component (well, pipe) that owns this variable

public:
    /*!
     * \brief Set m_id to next_id; set m_name to "no_name".
     *
     * TODO: Should take a Component pointer a parameter
     */
    Variable();

    void setName(const QString& name) { m_name = name; } //!< Set the identifying name for this variable.
    const QString& name() const { return m_name; } //!< Get function for m_name
    int id() { return m_id; }                      //!< Get function for m_id
    // Component* parent() {return p_parent;} // TODO: Return function for parent Component

protected:
    /*!
     * \brief Convenience class used by the variable subclasses to emit messages.
     * \param severity The severity of the event.
     * \param type The type of exception.
     * \param message The message to be printed.
     */
    void emitException(ExceptionSeverity severity, ExceptionType type, QString message);

};

#endif // VARIABLE_H
