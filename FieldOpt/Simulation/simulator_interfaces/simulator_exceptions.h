/******************************************************************************
 *
 *
 *
 * Created: 12.11.2015 2015 by einar
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

#ifndef SIMULATOR_EXCEPTIONS
#define SIMULATOR_EXCEPTIONS

#include <stdexcept>
#include <string>
#include <QString>

using std::string;

namespace Simulation {
namespace SimulatorInterfaces {

class OutputDirectoryDoesNotExistException : public std::runtime_error {
public:
    OutputDirectoryDoesNotExistException(const QString path)
        : std::runtime_error("The specified output directory does not exist: " + path.toStdString()) {}
};

class DriverFileDoesNotExistException : public std::runtime_error {
public:
    DriverFileDoesNotExistException(const QString path)
        : std::runtime_error("The specified driver file does not exist: " + path.toStdString()) {}
};

class DriverFileInvalidException : public std::runtime_error {
public:
    DriverFileInvalidException(const QString message)
        : std::runtime_error(message.toStdString()) {}
};

class UnableToWriteDriverFileException : public std::runtime_error {
public:
    UnableToWriteDriverFileException(const QString message)
        : std::runtime_error(message.toStdString()) {}
};

class UnableToFindKeywordException : public std::runtime_error {
public:
    UnableToFindKeywordException(const QString keyword)
        : std::runtime_error("Unable to find keyword in driver file: " + keyword.toStdString()) {}
};

}
}

#endif // SIMULATOR_EXCEPTIONS
