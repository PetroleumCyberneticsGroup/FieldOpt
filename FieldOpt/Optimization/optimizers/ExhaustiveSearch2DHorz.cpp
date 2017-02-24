/******************************************************************************
   Copyright (C) 2015-2016 Mathias C. Bellout <mathias.bellout@ntnu.no>

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
#include "ExhaustiveSearch2DHorz.h"

namespace Optimization {
namespace Optimizers {

ExhaustiveSearch2DHorz::ExhaustiveSearch2DHorz(
    Settings::Optimizer *settings, Case *base_case,
    Model::Properties::VariablePropertyContainer *variables,
    Reservoir::Grid::Grid *grid)
    : Optimizer(settings, base_case, variables, grid) {

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