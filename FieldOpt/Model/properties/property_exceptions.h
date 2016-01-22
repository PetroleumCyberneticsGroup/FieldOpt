/******************************************************************************
 *
 * variable_exceptions.h
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

#ifndef PROPERTY_EXCEPTIONS_H
#define PROPERTY_EXCEPTIONS_H

#include <stdexcept>
#include <string>

using std::string;

namespace Model {
namespace Properties {

class PropertyLockedException : public std::runtime_error {
public:
    PropertyLockedException(const string& message)
        : std::runtime_error(message) {}
};

class VariableIdDoesNotExistException : public std::runtime_error {
public:
    VariableIdDoesNotExistException(const string& message)
        : std::runtime_error(message) {}
};

class VariablePropertyHandlerCannotFindObjectException : public std::runtime_error {
public:
    VariablePropertyHandlerCannotFindObjectException(const string &message)
        : std::runtime_error(message) {}
};

class VariableTypeNotRecognizedException : public std::runtime_error {
public:
    VariableTypeNotRecognizedException()
        : std::runtime_error("The variable type was not recognized.") {}
};

}
}

#endif // PROPERTY_EXCEPTIONS_H
