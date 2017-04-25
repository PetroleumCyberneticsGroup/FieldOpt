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
#include "gss_patterns.hpp"

using namespace Eigen;

namespace Optimization {
namespace Optimizers {

WISmooothnessTest::WISmooothnessTest(
    ::Settings::Optimizer *settings,
    Case *base_case,
    ::Model::Properties::VariablePropertyContainer *variables,
    Reservoir::Grid::Grid *grid,
    Logger *logger
)
    : GSS(settings, base_case, variables, grid, logger) {

    // single out x direction from compass search dir matrix
    directions_ = GSSPatterns::Compass(num_vars_);
    xdirections_ = directions_[0];

    step_lengths_ = VectorXd(xdirections_.size());
    step_lengths_.fill(settings->parameters().initial_step_length);
}

void WISmooothnessTest::iterate()
{
    if (!is_successful_iteration() && iteration_ != 0)
        contract();
    case_handler_->AddNewCases(generate_trial_points());
    case_handler_->ClearRecentlyEvaluatedCases();
    iteration_++;
}

bool WISmooothnessTest::is_successful_iteration() {
    return case_handler_->RecentlyEvaluatedCases().contains(GetTentativeBestCase());
}

}
}

