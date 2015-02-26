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
#include "model/components/component.h"

class Component;

class Variable //!< Base class for all variables.
{

private:
    QString m_name;        //!< Identifying name for the variable
    int m_id;              //!< Unique id number for the variable
    static int next_id;    //!< Id to be set for next variable
    Component *p_parent;   //!< Pointer to the component (well, pipe) that owns this variable

public:
    Variable(Component *parent);  //!< Set m_id to next_id; set m_name to "no_name".

    void setName(const QString& name) { m_name = name; } //!< Set the identifying name for this variable.
    const QString& name() const { return m_name; } //!< Get function for m_name
    int id() { return m_id; }                      //!< Get function for m_id
    Component* parent() {return p_parent;} //!< \todo Return function for parent Component

    virtual bool isVariable() = 0;  //!< Checks if this is a variable value, i.e. if the bounds are not equal to the value.
};

#endif // VARIABLE_H
