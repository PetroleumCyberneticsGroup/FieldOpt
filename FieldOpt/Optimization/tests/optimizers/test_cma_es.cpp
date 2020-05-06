/******************************************************************************
   Created by Brage on 18/06/19.
   Copyright (C) 2019 Brage Strand Kristoffersen <brage_sk@hotmail.com>

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
#include "Optimization/optimizers/CMA_ES.h"
#include "Optimization/tests/test_resource_optimizer.h"
#include "Reservoir/tests/test_resource_grids.h"
#include "Optimization/tests/test_resource_test_functions.h"

using namespace TestResources::TestFunctions;
using namespace Optimization::Optimizers;

namespace {

    class CMA_ESTest : public ::testing::Test,
                       public TestResources::TestResourceOptimizer,
                       public TestResources::TestResourceGrids
    {
    protected:
        CMA_ESTest() {
            base_ = base_case_;
        }
        virtual ~CMA_ESTest() {}
        virtual void SetUp() {}

        Optimization::Case *base_;

    };

    TEST_F(CMA_ESTest, Constructor) {
    }

    TEST_F(CMA_ESTest, TestFunctionSpherical) {
//    test_case_2r_->set_objective_function_value(abs(Sphere(test_case_2r_->GetRealVarVector())));
//    test_case_ga_spherical_30r_->set_objective_function_value(abs(Sphere(test_case_ga_spherical_30r_->GetRealVarVector())));
        test_case_ga_spherical_6r_->set_objective_function_value(abs(Sphere(test_case_ga_spherical_6r_->GetRealVarVector())));
        settings_cma_es_min_->SetRngSeed(5);
        Optimization::Optimizer *minimizer = new CMA_ES(settings_cma_es_min_,
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
        EXPECT_NEAR(0.0, best_case->objective_function_value(), 0.12);
        EXPECT_NEAR(0.0, best_case->GetRealVarVector()[0], 0.12);
        EXPECT_NEAR(0.0, best_case->GetRealVarVector()[1], 0.12);
    }

    TEST_F(CMA_ESTest, TestFunctionRosenbrock) {
        test_case_ga_spherical_6r_->set_objective_function_value(abs(Rosenbrock(test_case_ga_spherical_6r_->GetRealVarVector())));
        settings_cma_es_min_->SetRngSeed(5);
        Optimization::Optimizer *minimizer = new CMA_ES(settings_cma_es_min_,
            test_case_ga_spherical_6r_, varcont_6r_, grid_5spot_, logger_ );

        while (!minimizer->IsFinished()) {
            auto next_case = minimizer->GetCaseForEvaluation();
            next_case->set_objective_function_value(Rosenbrock(next_case->GetRealVarVector()));
            minimizer->SubmitEvaluatedCase(next_case);
        }
        auto best_case = minimizer->GetTentativeBestCase();

        EXPECT_NEAR(0.0, best_case->objective_function_value(), 5);
        EXPECT_NEAR(1.0, best_case->GetRealVarVector()[0], 3);
        EXPECT_NEAR(1.0, best_case->GetRealVarVector()[1], 3);
    }

}


