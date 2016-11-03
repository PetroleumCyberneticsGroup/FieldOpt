/******************************************************************************
   Created by einar on 11/3/16.
   Copyright (C) 2016 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "GSS.h"

namespace Optimization {
    namespace Optimizers {

        GSS::GSS(Settings::Optimizer *settings, Case *base_case,
                 Model::Properties::VariablePropertyContainer *variables, Reservoir::Grid::Grid *grid)
                : Optimizer(settings, base_case, variables, grid) {
            step_tol_ = settings->parameters().minimum_step_length;
            // \TODO Contr fac -> Set in subclass
            // \TODO Expan fac -> Set in subclass
            // \TODO Directions -> Set in subclass
            // \TODO Step length -> Resize to directions size, set all to initial step length
        }

    }
}
