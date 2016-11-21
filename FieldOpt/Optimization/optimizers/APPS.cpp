/******************************************************************************
   Created by einar on 11/21/16.
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
#include "APPS.h"

namespace Optimization {
    namespace Optimizers {

        APPS::APPS(Settings::Optimizer *settings, Case *base_case,
                   Model::Properties::VariablePropertyContainer *variables, Reservoir::Grid::Grid *grid)
                : GSS(settings, base_case, variables, grid) {

        }

        void APPS::handleEvaluatedCase(Case *c) {

        }

        void APPS::iterate() {

        }
    }
}
