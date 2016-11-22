/******************************************************************************
   Created by einar on 11/22/16.
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

#include <gtest/gtest.h>
#include <tests/test_resource_optimizer.h>
#include <Reservoir/tests/test_resource_grids.h>
#include "optimizers/APPS.h"
#include "tests/test_resource_test_functions.h"
#include "Utilities/math.hpp"

using namespace TestResources::TestFunctions;
using namespace Optimization::Optimizers;

namespace {
    class APPSTest : public ::testing::Test,
                     public ::TestResources::TestResourceOptimizer,
                     public ::TestResources::TestResourceGrids
    {
    protected:
        APPSTest() {}

        virtual ~APPSTest() {}

        Optimization::Optimizer *apps_minimizer_;
        Optimization::Optimizer *apps_maximizer_;
    };

    TEST_F(APPSTest, Constructor) {
        test_case_2r_->set_objective_function_value(1.0);
        apps_minimizer_ = new APPS(settings_apps_min_unconstr_,
                                   test_case_2r_,
                                   varcont_prod_bhp_,
                                   grid_5spot_);
    }

    TEST_F(APPSTest, GetNewCases) {
        test_case_1_3i_->set_objective_function_value(Sphere(test_case_1_3i_->GetRealVarVector()));
        Optimization::Optimizer *maximizer = new APPS(settings_apps_max_unconstr_,
                                                      test_case_1_3i_, varcont_prod_bhp_, grid_5spot_);

        // These four cases should change the values of the two first int vars, +50 then -50
        Optimization::Case *new_case_1 = maximizer->GetCaseForEvaluation();
        Optimization::Case *new_case_2 = maximizer->GetCaseForEvaluation();
        Optimization::Case *new_case_3 = maximizer->GetCaseForEvaluation();
        Optimization::Case *new_case_4 = maximizer->GetCaseForEvaluation();
        EXPECT_FALSE(new_case_1->id() == new_case_2->id());
        EXPECT_FALSE(new_case_3->id() == new_case_4->id());

        EXPECT_EQ(test_case_1_3i_->GetIntegerVarVector()[0] + 8, new_case_1->GetIntegerVarVector()[0]);
        EXPECT_EQ(test_case_1_3i_->GetIntegerVarVector()[1] + 0, new_case_1->GetIntegerVarVector()[1]);
        EXPECT_EQ(test_case_1_3i_->GetIntegerVarVector()[2] + 0, new_case_1->GetIntegerVarVector()[2]);

        EXPECT_EQ(test_case_1_3i_->GetIntegerVarVector()[1] + 8, new_case_2->GetIntegerVarVector()[1]);
        EXPECT_EQ(test_case_1_3i_->GetIntegerVarVector()[2] + 8, new_case_3->GetIntegerVarVector()[2]);
        EXPECT_EQ(test_case_1_3i_->GetIntegerVarVector()[0] - 8, new_case_4->GetIntegerVarVector()[0]);
    }

    TEST_F(APPSTest, TestFunctionSpherical) {
        test_case_2r_->set_objective_function_value(Sphere(test_case_2r_->GetRealVarVector()));
        Optimization::Optimizer *minimizer = new APPS(settings_apps_min_unconstr_,
                                                      test_case_2r_, varcont_prod_bhp_, grid_5spot_);

        QList<Optimization::Case *> under_eval = QList<Optimization::Case *>();
        for (int i = 0; i < 4; ++i)
            under_eval.append(minimizer->GetCaseForEvaluation());

        while (!minimizer->IsFinished()) {
            auto random_evaluated_case = under_eval.takeAt(random_integer(0, 3));
            random_evaluated_case->set_objective_function_value(Sphere(random_evaluated_case->GetRealVarVector()));
            minimizer->SubmitEvaluatedCase(random_evaluated_case);
            under_eval.append(minimizer->GetCaseForEvaluation());
        }
        auto best_case = minimizer->GetTentativeBestCase();
        EXPECT_NEAR(0.0, best_case->objective_function_value(), 0.01);
        EXPECT_NEAR(0.0, best_case->GetRealVarVector()[0], 0.01);
        EXPECT_NEAR(0.0, best_case->GetRealVarVector()[1], 0.01);
    }
}