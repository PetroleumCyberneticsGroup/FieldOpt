/******************************************************************************
   Created by einar on 6/7/17.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "Optimization/tests/test_resource_optimizer.h"
#include "Reservoir/tests/test_resource_grids.h"
#include "Optimization/tests/test_resource_test_functions.h"
#include "Optimization/optimizers/bayesian_optimization/EGO.h"
#include "gp/rprop.h"
#include "Optimization/normalizer.h"
#include "Utilities/math.hpp"
#include "Utilities/stringhelpers.hpp"

using namespace TestResources::TestFunctions;
using namespace Optimization::Optimizers;
using namespace std;

namespace {

class EGOTest : public ::testing::Test,
                public TestResources::TestResourceOptimizer,
                public TestResources::TestResourceGrids
{
 protected:
  EGOTest() {
      base_ = base_case_;
  }
  virtual ~EGOTest() {}
  virtual void SetUp() {}

  Optimization::Case *base_;

};

TEST_F(EGOTest, Constructor) {
    test_case_ga_spherical_6r_->set_objective_function_value(abs(Sphere(test_case_ga_spherical_6r_->GetRealVarVector())));
    auto ego = BayesianOptimization::EGO(settings_ego_max_, test_case_ga_spherical_6r_, varcont_6r_, grid_5spot_, logger_);
}

TEST_F(EGOTest, GaussianProcess) {
    libgp::GaussianProcess gp(2, "CovMatern5iso");
    cout << "Optimizing hyperparameters" << endl;
    Eigen::VectorXd params(2);
    params << 1, 1;
    gp.covf().set_loghyper(params);

    auto gen = get_random_generator();

    cout << "Log likelihood: " << gp.log_likelihood() << endl;

    // Train using random points from the Sphere function
    for (int i = 0; i < 100; ++i) {
        Eigen::VectorXd rands = random_doubles_eigen(gen, -10, 10, 2);
        double f = Sphere(rands);
        gp.add_pattern(rands.data(), f);
    }

    cout << "Log likelihood: " << gp.log_likelihood() << endl;

    params(0) = -1; params(1) = -1;
    gp.covf().set_loghyper(params);
    libgp::RProp rprop;
    rprop.init();
    rprop.maximize(&gp, 50, 0);

    cout << "Log likelihood: " << gp.log_likelihood() << endl;

    // Check consistency
    for (int i = 0; i < 10; ++i) {
        Eigen::VectorXd rands = random_doubles_eigen(gen, -10, 10, 2);
        double f = Sphere(rands);
        double expected_f = gp.f(rands.data());
        double uncert = gp.var(rands.data());
        cout << "Error: " << abs(expected_f-f) << "; Expected: " << expected_f << "; Actual: " << f << "; Uncert: " << uncert << endl;
    }

    //    params << -1, -1;
//    gp->covf().set_loghyper(params);
//    libgp::RProp rprop;
//    rprop.init();
//    rprop.maximize(gp, 50, 0);
}


TEST_F(EGOTest, SingleIteration) {
    test_case_ga_spherical_6r_->set_objective_function_value(- abs(Sphere(test_case_ga_spherical_6r_->GetRealVarVector())));
    Optimization::Optimizer *ego = new BayesianOptimization::EGO(settings_ego_max_,
                                                                 test_case_ga_spherical_6r_,
                                                                 varcont_6r_,
                                                                 grid_5spot_,
                                                                 logger_
    );

    // Get the initial guesses
    for (int i = 0; i < 20; ++i) {
        auto next_case = ego->GetCaseForEvaluation();
        next_case->set_objective_function_value(- abs(Sphere(next_case->GetRealVarVector())));
        cout << next_case->objective_function_value() << endl;
        ego->SubmitEvaluatedCase(next_case);
    }

    // Get the first computed case
    auto next_case = ego->GetCaseForEvaluation();
    cout << next_case->objective_function_value() << endl;
}

//TEST_F(EGOTest, TestFunctionSpherical) {
//    test_case_ga_spherical_6r_->set_objective_function_value(- abs(Sphere(test_case_ga_spherical_6r_->GetRealVarVector())));
//    Optimization::Optimizer *ego = new BayesianOptimization::EGO(settings_ego_max_,
//                                                                 test_case_ga_spherical_6r_,
//                                                                 varcont_6r_,
//                                                                 grid_5spot_,
//                                                                 logger_
//    );
//
//    while (ego->IsFinished() == Optimization::Optimizer::TerminationCondition::NOT_FINISHED) {
//        auto next_case = ego->GetCaseForEvaluation();
//        next_case->set_objective_function_value(- abs(Sphere(next_case->GetRealVarVector())));
//        next_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_DONE;
//        ego->SubmitEvaluatedCase(next_case);
//    }
//    auto best_case = ego->GetTentativeBestCase();
//    EXPECT_NEAR(0.0, best_case->objective_function_value(), 0.1);
//    EXPECT_NEAR(0.0, best_case->GetRealVarVector()[0], 0.1);
//    EXPECT_NEAR(0.0, best_case->GetRealVarVector()[1], 0.1);
//}


}

