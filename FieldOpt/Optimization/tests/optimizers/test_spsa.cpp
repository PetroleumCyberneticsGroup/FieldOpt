/******************************************************************************
   Created by einar on 1/15/19.
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include <gtest/gtest.h>
#include "Runner/tests/test_resource_runner.hpp"
#include "optimizers/SPSA.h"
#include "Optimization/tests/test_resource_optimizer.h"
#include "Reservoir/tests/test_resource_grids.h"
#include "Optimization/tests/test_resource_test_functions.h"

using namespace TestResources::TestFunctions;
using namespace Optimization::Optimizers;

namespace {

class SPSATest : public ::testing::Test,
                             public TestResources::TestResourceOptimizer,
                             public TestResources::TestResourceGrids
{
 protected:
  SPSATest() {
      base_ = base_case_;
  }
  virtual ~SPSATest() {}
  virtual void SetUp() {}

  Optimization::Case *base_;

};

TEST_F(SPSATest, Constructor) {
}

TEST_F(SPSATest, TestFunctionSphericalMinimize) {
    test_case_ga_spherical_6r_->set_objective_function_value(abs(Sphere(test_case_ga_spherical_6r_->GetRealVarVector())));
    Optimization::Optimizer *minimizer = new SPSA(settings_spsa_min_,
                                                    test_case_ga_spherical_6r_,
                                                    varcont_6r_,
                                                    grid_5spot_,
                                                    logger_
    );

    while (!minimizer->IsFinished()) {
        auto next_case = minimizer->GetCaseForEvaluation();
        next_case->set_objective_function_value(abs(Sphere(next_case->GetRealVarVector())));
        next_case->state.eval = Optimization::Case::CaseState::E_DONE;
        minimizer->SubmitEvaluatedCase(next_case);
    }
    auto best_case = minimizer->GetTentativeBestCase();
    EXPECT_NEAR(0.0, best_case->objective_function_value(), 0.1);
    EXPECT_NEAR(0.0, best_case->GetRealVarVector()[0], 0.1);
    EXPECT_NEAR(0.0, best_case->GetRealVarVector()[1], 0.1);
}

TEST_F(SPSATest, TestFunctionSphericalMaximize) {
    test_case_ga_spherical_6r_->set_objective_function_value(-1*abs(Sphere(test_case_ga_spherical_6r_->GetRealVarVector())));
    Optimization::Optimizer *maximizer = new SPSA(settings_spsa_max_,
                                                    test_case_ga_spherical_6r_,
                                                    varcont_6r_,
                                                    grid_5spot_,
                                                    logger_
    );

    while (!maximizer->IsFinished()) {
        auto next_case = maximizer->GetCaseForEvaluation();
        next_case->set_objective_function_value(-1*abs(Sphere(next_case->GetRealVarVector())));
        next_case->state.eval = Optimization::Case::CaseState::E_DONE;
        maximizer->SubmitEvaluatedCase(next_case);
    }
    auto best_case = maximizer->GetTentativeBestCase();
    EXPECT_NEAR(0.0, best_case->objective_function_value(), 0.1);
    EXPECT_NEAR(0.0, best_case->GetRealVarVector()[0], 0.1);
    EXPECT_NEAR(0.0, best_case->GetRealVarVector()[1], 0.1);
}

}

