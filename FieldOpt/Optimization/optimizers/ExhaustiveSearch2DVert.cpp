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

#include <iostream>
#include "ExhaustiveSearch2DVert.h"

namespace Optimization {
namespace Optimizers {

ExhaustiveSearch2DVert::ExhaustiveSearch2DVert(Settings::Optimizer *settings,
                                               Case *base_case,
                                               Model::Properties::VariablePropertyContainer *variables,
                                               Reservoir::Grid::Grid *grid,
                                               Logger *logger
)
    : Optimizer(settings, base_case, variables, grid, logger) {
    grid_ = grid;

    if (variables->BinaryVariableSize() != 0 || variables->ContinousVariableSize() != 0
        || variables->DiscreteVariableSize() != 3)
        throw std::runtime_error("ExhaustiveSearch2DVert: Expecting to receive exactly three discrete variables.");

    auto blockvars = variables->GetWellBlockVariables();
    if (blockvars.size() != 3)
        throw std::runtime_error("ExhaustiveSearch2DVert: Error getting variables.");

    for (auto prop : blockvars) {
        if (prop->propertyInfo().coord == Model::Properties::Property::Coordinate::i)
            i_varid = prop->id();
        else if (prop->propertyInfo().coord == Model::Properties::Property::Coordinate::j)
            j_varid = prop->id();
        else if (prop->propertyInfo().coord == Model::Properties::Property::Coordinate::k)
            continue;
        else
            throw std::runtime_error("ExhaustiveSearch2DVert: Error getting variables.");
    }
}
Optimizer::TerminationCondition ExhaustiveSearch2DVert::IsFinished() {
    if (iteration_ == 0) return NOT_FINISHED;
    else if (case_handler_->QueuedCases().size() > 0) return NOT_FINISHED;
    else return MAX_EVALS_REACHED;
}
void ExhaustiveSearch2DVert::iterate() {
    int i_max = grid_->Dimensions().nx;
    int j_max = grid_->Dimensions().ny;

    if (iteration_ > 0) {
        std::cout << "Can't iterate more than once." << std::endl;
        return;
    }

    for (int i = 1; i < i_max; ++i) {
        for (int j = 1; j < j_max; ++j) {
            auto new_case = new Case(GetTentativeBestCase());
            new_case->set_integer_variable_value(i_varid, i);
            new_case->set_integer_variable_value(j_varid, j);
            case_handler_->AddNewCase(new_case);
        }
    }
}

}
}
