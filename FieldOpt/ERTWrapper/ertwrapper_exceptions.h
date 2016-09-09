/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef ECLGRIDREADER_EXCEPTIONS_H
#define ECLGRIDREADER_EXCEPTIONS_H

#include <stdexcept>
#include <string>

using std::string;

namespace ERTWrapper {

class GridNotReadException : public std::runtime_error {
public:
    GridNotReadException(const string& message)
        : std::runtime_error(message) {}
};

class InvalidIndexException : public std::runtime_error {
public:
    InvalidIndexException(const string& message)
        : std::runtime_error(message) {}
};

class SummaryFileNotFoundAtPathException : public std::runtime_error {
public:
    SummaryFileNotFoundAtPathException(const string &path)
        : std::runtime_error("No valid simulation case found at path " + path) {}
};

class SummaryVariableDoesNotExistException : public std::runtime_error {
public:
    SummaryVariableDoesNotExistException(const string& message)
        : std::runtime_error(message) {}
};

class SummaryTimeStepDoesNotExistException : public std::runtime_error {
public:
    SummaryTimeStepDoesNotExistException(const string& message)
        : std::runtime_error(message) {}
};

}

#endif // ECLGRIDREADER_EXCEPTIONS_H
