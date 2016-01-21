/******************************************************************************
 *
 *
 *
 * Created: 12.10.2015 2015 by einar
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

#ifndef RESULTS_EXCEPTIONS
#define RESULTS_EXCEPTIONS

#include <stdexcept>
#include <string>

using std::string;

namespace Simulation { namespace Results {

class ResultFileNotFoundException : public std::runtime_error {
public:
    ResultFileNotFoundException(const string &path)
        : std::runtime_error("No valid result file found at path" + path) {}
};

class ResultsNotAvailableException : public std::runtime_error {
public:
    ResultsNotAvailableException()
        : std::runtime_error("Results are not currently available.") {}
};

class ResultPropertyKeyDoesNotExistException : public std::runtime_error {
public:
    ResultPropertyKeyDoesNotExistException(const string &simulator)
        : std::runtime_error("The requested property key cannot be retrieved from " + simulator + "summaries.") {}
};

class ResultTimeIndexInvalidException : public std::runtime_error {
public:
    ResultTimeIndexInvalidException(const int index)
        : std::runtime_error("Time index " + std::to_string(index) + " is not valid for the current results.") {}
};

}}

#endif // RESULTS_EXCEPTIONS
