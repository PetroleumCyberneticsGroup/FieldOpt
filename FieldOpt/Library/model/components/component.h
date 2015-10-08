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

#ifndef COMPONENT_H
#define COMPONENT_H

#include <QVector>
#include <QString>
#include "Library/model/stream.h"
#include "Library/exceptionhandler/componenthandler.h"

/*!
 * \brief Base class for components.
 *
 * Examples of components are pipes, separators and wells etc. I.e. all parts that have a Stream attached to them.
 */
class Component : public QObject
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
    QVector<Stream*> m_streams;  //!< The streams passing through the component.
    int m_id;                    //!< Unique id for the component.
    static int next_id;          //!< The id to be set for the next component. Initialized to 0 and incremented when the constructor is called.
public:
    Component();  //!< Set m_id.

    /*!
     * \brief Copy constructor.
     *
     * Sets m_id to the id of the component to be copied;
     * Clears the Stream vector and resizes it to the size of the parameter component;
     * Fills the Stream vector with copies of the Streams in the input component.
     * \param component The component to be copied.
     */
    Component(const Component& component);
    virtual ~Component();

    void clearStreams() { m_streams.clear(); }                       //!< Clear the Stream vector.
    void addStream(Stream* stream) { m_streams.push_back(stream); }  //!< Add a Stream at position the end of thre Stream vector.

    /*!
     * \brief Set a new Stream at position i in the Stream vector.
     *
     * \todo Check that the number of streams correspond to the number of schedule entries.
     * \todo Check that the time of the stream corresponds to the time of the schedule entry.
     *
     * \param i The index to set a new Stream for.
     * \param stream The Stream to be set at index i.
     * \return Returns true if the Stream at position i in m_streams was successfully changed to a new one; otherwise false.
     */
    bool setStream(int i, Stream* stream);

    int numberOfStreams() { return m_streams.size(); }  //!< Get size of the Stream vector.
    Stream* stream(int i) { return m_streams.at(i); }   //!< Get the Stream at position i in the Stream vector.
    QVector<Stream*> streams() {return m_streams; }     //!< Get all Streams contained in the Stream vector.
    int id() { return m_id; }                           //!< Get the unique id for this stream.

    virtual QString description() const = 0; //!< Should return a string describing the component.

protected:
    /*!
     * \brief Convenience class used by the component subclasses to emit exceptions.
     * \param severity The severity of the event.
     * \param type The type of exception.
     * \param message The message to be printed.
     */
    void emitException(ExceptionSeverity severity, ExceptionType type, QString message);
};

#endif // COMPONENT_H
