/******************************************************************************
   Created by einar on 2/26/19.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#ifndef FIELDOPT_SYSTEM_H
#define FIELDOPT_SYSTEM_H

#include <string>
#include <stdlib.h>

/*!
 * @brief Check whether an environment variable has been set.
 * @param var_name Name of variable to check (e.g. FIELDOPT_BUILD_ROOT).
 * @return True if the variable has been set, otherwise false.
 */
inline bool is_env_var_set(std::string var_name) {
    char const* var_value = getenv(var_name.c_str());
    if (var_value == NULL) {
        return false;
    }
    else {
        return true;
    }
}

/*!
 * @brief Get the value of an environment variable. Throws an exception if the variable is unset.
 * @param var_name Name of variable to check (e.g. FIELDOPT_BUILD_ROOT).
 * @return Value of environment variable as string.
 */
inline std::string get_env_var_value(std::string var_name) {
    if (!is_env_var_set(var_name)) {
        throw std::runtime_error("Attempting to get unset environment variable " + var_name);
    }
    else {
        char const* var_value = getenv(var_name.c_str());
        std::string s(var_value);
        return var_value;
    }
}

#endif //FIELDOPT_SYSTEM_H
