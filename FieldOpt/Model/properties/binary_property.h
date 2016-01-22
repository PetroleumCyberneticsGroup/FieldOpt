/******************************************************************************
 *
 * binaryvariable.h
 *
 * Created: 23.09.2015 2015 by einar
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

#ifndef BINARY_PROPERTY_H
#define BINARY_PROPERTY_H

#include "property.h"

namespace Model {
namespace Properties {

/*!
 * \brief The BinaryProperty class describes a binary property in the model.
 * These are typically on/off or open/shut switches when optimizing.
 */
class BinaryProperty : public Property
{
public:
    BinaryProperty(bool value);

    bool value() const { return value_; }
    void setValue(bool value);
    QString ToString() const;

private:
    bool value_;
};

}
}


#endif // BINARY_PROPERTY_H
