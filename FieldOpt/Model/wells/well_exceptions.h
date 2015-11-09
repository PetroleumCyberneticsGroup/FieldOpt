/******************************************************************************
 *
 *
 *
 * Created: 27.10.2015 2015 by einar
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

#ifndef WELL_EXCEPTIONS
#define WELL_EXCEPTIONS

#include <stdexcept>
#include <string>

using std::string;

namespace Model {
namespace Wells {

class WellCompletionNotRecognizedException : public std::runtime_error {
public:
    WellCompletionNotRecognizedException(const string& message)
        : std::runtime_error(message) {}
};

class CompletionNotDefinedForWellBlockException : public std::runtime_error {
public:
    CompletionNotDefinedForWellBlockException(const int i, const int j, const int k)
        : std::runtime_error("No completion defined for well block (" +
                             std::to_string(i) + ", " +
                             std::to_string(j) + ", " +
                             std::to_string(k) + ").") {}
};

class PerforationNotDefinedForWellBlockException : public std::runtime_error {
public:
    PerforationNotDefinedForWellBlockException(const int i, const int j, const int k)
        : std::runtime_error("No perforation defined for well block (" +
                             std::to_string(i) + ", " +
                             std::to_string(j) + ", " +
                             std::to_string(k) + ").") {}
};

class WellBlockNotFoundException : public std::runtime_error {
public:
    WellBlockNotFoundException(const int i, const int j, const int k)
        : std::runtime_error("No well block defined at (" +
                             std::to_string(i) + ", " +
                             std::to_string(j) + ", " +
                             std::to_string(k) + ").") {}
    WellBlockNotFoundException(const int id)
        : std::runtime_error("No well block found with id " + std::to_string(id)) {}
};

}
}

#endif // WELL_EXCEPTIONS
