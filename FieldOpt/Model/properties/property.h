/******************************************************************************
 *
 * variable.h
 *
 * Created: 22.09.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
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
 *****************************************************************************/

#ifndef PROPERTY_H
#define PROPERTY_H

#include "property_exceptions.h"
#include <QString>
#include <QUuid>

namespace Model {
namespace Properties {

/*!
 * \brief The Property class is an abstract class implemented by
 * specific property types, i.e. integer, real and binary. It holds
 * and to some extends describes the value of a property in the model.
 */
class Property
{
public:
    enum Type { Discrete, Continous, Binary };

    Type type() const { return type_; }
    QString name() const { return name_;}
    void setName(QString name) { name_ = name; }

    bool IsLocked() const { return locked_; }
    void Lock() { locked_ = true; }
    void Unlock() { locked_ = false; }

    QUuid id() const { return id_; }

protected:
    Property(Type type) { type_ = type; locked_ = false; }

private:
    QUuid id_ = QUuid::createUuid();
    Type type_;
    bool locked_;
    QString name_;
};

}
}

#endif // PROPERTY_H
