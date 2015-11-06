/******************************************************************************
 *
 * settings_exceptions.h
 *
 * Created: 28.09.2015 2015 by einar
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

#ifndef SETTINGS_EXCEPTIONS_H
#define SETTINGS_EXCEPTIONS_H

#include <stdexcept>
#include <string>

using std::string;

namespace Utilities {
namespace Settings {

class FileNotFoundException : public std::runtime_error {
public:
    FileNotFoundException(const string &path)
        : std::runtime_error("No file or folder found at " + path) {}
};

class DriverFilePathNotSpecifiedException : public std::runtime_error {
public:
    DriverFilePathNotSpecifiedException(const string &message)
        : std::runtime_error(message) {}
};

class DriverFileReadException : public std::runtime_error {
public:
    DriverFileReadException(const string &message)
        : std::runtime_error(message) {}
};

class DriverFileJsonParsingException : public std::runtime_error {
public:
    DriverFileJsonParsingException(const string &message)
        : std::runtime_error(message) {}
};

class DriverFileFormatException : public std::runtime_error {
public:
    DriverFileFormatException(const string &message)
        : std::runtime_error(message) {}
};

class UnableToParseGlobalSectionException : public std::runtime_error {
public:
    UnableToParseGlobalSectionException(const string &message)
        : std::runtime_error(message) {}
};

class UnableToParseSimulatorSectionException : public std::runtime_error {
public:
    UnableToParseSimulatorSectionException(const string &message)
        : std::runtime_error(message) {}
};

class SimulatorTypeNotRecognizedException : public std::runtime_error {
public:
    SimulatorTypeNotRecognizedException(const string &message)
        : std::runtime_error(message) {}
};

class NoSimulatorCommandsGivenException : public std::runtime_error {
public:
    NoSimulatorCommandsGivenException(const string &message)
        : std::runtime_error(message) {}
};

class UnableToParseOptimizerSectionException : public std::runtime_error {
public:
    UnableToParseOptimizerSectionException(const string &message)
        : std::runtime_error(message) {}
};

class OptimizerTypeNotRecognizedException : public std::runtime_error {
public:
    OptimizerTypeNotRecognizedException(const string &message)
        : std::runtime_error(message) {}
};

class UnableToParseOptimizerParametersSectionException : public std::runtime_error {
public:
    UnableToParseOptimizerParametersSectionException(const string &message)
        : std::runtime_error(message) {}
};

class UnableToParseOptimizerObjectiveSectionException : public std::runtime_error {
public:
    UnableToParseOptimizerObjectiveSectionException(const string &message)
        : std::runtime_error(message) {}
};

class UnableToParseOptimizerConstraintsSectionException : public std::runtime_error {
public:
    UnableToParseOptimizerConstraintsSectionException(const string &message)
        : std::runtime_error(message) {}
};

class UnableToParseModelSectionException : public std::runtime_error {
public:
    UnableToParseModelSectionException(const string &message)
        : std::runtime_error(message) {}
};

class UnableToParseReservoirModelSectionException : public std::runtime_error {
public:
    UnableToParseReservoirModelSectionException(const string &message)
        : std::runtime_error(message) {}
};

class UnableToParseWellsModelSectionException : public std::runtime_error {
public:
    UnableToParseWellsModelSectionException(const string &message)
        : std::runtime_error(message) {}
};

class CompletionDefinedOutsideWellException : public std::runtime_error {
public:
    CompletionDefinedOutsideWellException(const string well_name)
        : std::runtime_error("Illegal definition of completion outside the set of well blocks for well " + well_name) {}
};

class DriverFileInconsistentException : public std::runtime_error {
public:
    DriverFileInconsistentException(const string &message)
        : std::runtime_error(message) {}
};

class WellBlockNotFoundException : public std::runtime_error {
public:
    WellBlockNotFoundException(const string &message)
        : std::runtime_error(message) {}
};

}
}
#endif // SETTINGS_EXCEPTIONS_H
