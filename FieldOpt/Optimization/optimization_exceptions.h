/******************************************************************************
 *
 *
 *
 * Created: 30.11.2015 2015 by einar
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

#ifndef OPTIMIZATION_EXCEPTIONS
#define OPTIMIZATION_EXCEPTIONS

#include <stdexcept>
#include <string>
#include <QString>

namespace Optimization {

class ObjectiveFunctionException : public std::runtime_error {
public:
    ObjectiveFunctionException(const QString &message)
        : std::runtime_error(message.toStdString()) {}
};

class VariableException : public std::runtime_error {
public:
    VariableException(const QString &message)
        : std::runtime_error(message.toStdString()) {}
};

class CaseHandlerException : public std::runtime_error {
public:
    CaseHandlerException(const QString &message)
        : std::runtime_error(message.toStdString()) {}
};

class OptimizerInitializationException : public std::runtime_error {
public:
    OptimizerInitializationException(const QString &message)
        : std::runtime_error("Unable to initialize the optimizer: " + message.toStdString()) {}
};

}

#endif // OPTIMIZATION_EXCEPTIONS

