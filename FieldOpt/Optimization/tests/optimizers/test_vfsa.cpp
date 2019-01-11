/******************************************************************************
   Created by einar on 1/11/19.
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
#include <Runner/tests/test_resource_runner.hpp>
#include "optimizers/VFSA.h"
#include "Optimization/tests/test_resource_optimizer.h"
#include "Reservoir/tests/test_resource_grids.h"
#include "Optimization/tests/test_resource_test_functions.h"

using namespace TestResources::TestFunctions;
using namespace Optimization::Optimizers;

namespace {

class VFSATest : public ::testing::Test,
                             public TestResources::TestResourceOptimizer,
                             public TestResources::TestResourceGrids
{
 protected:
  VFSATest() {
      base_ = base_case_;
  }
  virtual ~VFSATest() {}
  virtual void SetUp() {}

  Optimization::Case *base_;

};

TEST_F(VFSATest, Constructor) {
}

TEST_F(VFSATest, TestFunctionSpherical) {
//    test_case_2r_->set_objective_function_value(abs(Sphere(test_case_2r_->GetRealVarVector())));
//    test_case_vfsa_spherical_30r_->set_objective_function_value(abs(Sphere(test_case_vfsa_spherical_30r_->GetRealVarVector())));
    test_case_ga_spherical_6r_->set_objective_function_value(abs(Sphere(test_case_ga_spherical_6r_->GetRealVarVector())));
    Optimization::Optimizer *minimizer = new VFSA(settings_vfsa_min_,
                                                    test_case_ga_spherical_6r_,
                                                    varcont_6r_,
                                                    grid_5spot_,
                                                    logger_
    );

    while (!minimizer->IsFinished()) {
        auto next_case = minimizer->GetCaseForEvaluation();
        next_case->set_objective_function_value(abs(Sphere(next_case->GetRealVarVector())));
        minimizer->SubmitEvaluatedCase(next_case);
    }
    auto best_case = minimizer->GetTentativeBestCase();
//    EXPECT_NEAR(0.0, best_case->objective_function_value(), 0.1);
//    EXPECT_NEAR(0.0, best_case->GetRealVarVector()[0], 0.1);
//    EXPECT_NEAR(0.0, best_case->GetRealVarVector()[1], 0.1);
}

TEST_F(VFSATest, TestFunctionRosenbrock) {
    test_case_ga_spherical_6r_->set_objective_function_value(-1.0 * abs(Rosenbrock(test_case_ga_spherical_6r_->GetRealVarVector())));
    Optimization::Optimizer *maximizer = new VFSA(settings_vfsa_max_,
                                                    test_case_ga_spherical_6r_,
                                                    varcont_6r_,
                                                    grid_5spot_,
                                                    logger_
    );

    while (!maximizer->IsFinished()) {
        auto next_case = maximizer->GetCaseForEvaluation();
        next_case->set_objective_function_value(- 1.0 * abs(Rosenbrock(next_case->GetRealVarVector())));
        maximizer->SubmitEvaluatedCase(next_case);
    }
    auto best_case = maximizer->GetTentativeBestCase();

//    EXPECT_NEAR(0.0, best_case->objective_function_value(), 5);
//    EXPECT_NEAR(1.0, best_case->GetRealVarVector()[0], 2.5);
//    EXPECT_NEAR(1.0, best_case->GetRealVarVector()[1], 2.5);
}

}

