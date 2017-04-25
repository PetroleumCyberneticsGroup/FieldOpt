/******************************************************************************
   Copyright (C) 2017 Mathias C. Bellout <mathias.bellout@ntnu.no>

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

#include <iostream>
#include "wi_smoothness_test.h"

namespace Optimization {
namespace Optimizers {

WISmooothnessTest::WISmooothnessTest(
    Settings::Optimizer *settings, Case *base_case,
    Model::Properties::VariablePropertyContainer *variables,
    Reservoir::Grid::Grid *grid,
    Logger *logger)
    : Optimizer(settings, base_case, variables, grid, logger) {

    grid_ = grid;

    // Check problem has exactly six continuous (XYZ) variables
    if (variables->ContinousVariableSize() != 6
        || variables->DiscreteVariableSize() != 0
        || variables->BinaryVariableSize() != 0) {
        throw std::runtime_error(
                    "ExhaustiveSearch2DHrz: Expecting to "
                        "receive exactly six continuous (XYZ) "
                        "variables: (x,y,z)_heel (x,y,z)_toe.");
    }



}

}
}
