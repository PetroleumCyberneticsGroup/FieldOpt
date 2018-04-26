/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include "paths.h"

void Paths::SetPath(Paths::Path path, std::string path_string) {
    if (path >= 0 && !FileExists(path_string)) {
        throw std::runtime_error(Paths::GetPathDescription(path) + " not found at " + path_string);
    }
    else if (path < 0 && !DirectoryExists(path_string)) {
        throw std::runtime_error(Paths::GetPathDescription(path) + " not found at " + path_string);
    }
    else {
        paths_[path] = path_string;
    }
}
bool Paths::IsSet(Paths::Path path) {
    return paths_.count(path) > 0;
}
std::string Paths::GetPath(Paths::Path path) {
    if (!IsSet(path)) {
        return "UNSET";
    }
    else {
        return paths_[path];
    }
}
