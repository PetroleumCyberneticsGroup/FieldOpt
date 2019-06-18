/******************************************************************************
   Created by Brage on 14/06/19.
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
#include "Runner/tests/test_resource_runner.hpp"
#include "Optimization/tests/test_resource_optimizer.h"
#include "Reservoir/tests/test_resource_grids.h"
#include "Optimization/tests/test_resource_test_functions.h"
#include "Optimization/optimizers/bayesian_optimization/BO_PSO.h"
#include "gp/rprop.h"
#include "Optimization/normalizer.h"
#include "Utilities/math.hpp"
#include "Utilities/stringhelpers.hpp"

using namespace TestResources::TestFunctions;
using namespace Optimization::Optimizers;
using namespace std;

namespace {

    class BO_PSOTest : public ::testing::Test,
                    public TestResources::TestResourceOptimizer,
                    public TestResources::TestResourceGrids
    {
    protected:
        BO_PSOTest() {
            base_ = base_case_;
        }
        virtual ~BO_PSOTest() {}
        virtual void SetUp() {}

        Optimization::Case *base_;

    };

    TEST_F(BO_PSOTest, Constructor) {
        test_case_ga_spherical_6r_->set_objective_function_value(abs(Sphere(test_case_ga_spherical_6r_->GetRealVarVector())));
        auto bo_pso = BayesianOptimization::BO_PSO(settings_bo_ego_max_, test_case_ga_spherical_6r_, varcont_6r_, grid_5spot_, logger_);
    }

    TEST_F(BO_PSOTest, GaussianProcess) {
        libgp::GaussianProcess gp(2, "CovMatern5iso");
        cout << "Optimizing hyperparameters" << endl;
        Eigen::VectorXd params(2);
        params << 1, 1;
        gp.covf().set_loghyper(params);

        auto gen = get_random_generator(10);

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


    TEST_F(BO_PSOTest, SingleIteration) {
        test_case_ga_spherical_6r_->set_objective_function_value(- abs(Sphere(test_case_ga_spherical_6r_->GetRealVarVector())));
        Optimization::Optimizer *bo_pso = new BayesianOptimization::BO_PSO(settings_bo_ego_max_,
                                                                     test_case_ga_spherical_6r_,
                                                                     varcont_6r_,
                                                                     grid_5spot_,
                                                                     logger_
        );

        // Get the initial guesses
        for (int i = 0; i < 20; ++i) {
            auto next_case = bo_pso->GetCaseForEvaluation();
            next_case->set_objective_function_value(- abs(Sphere(next_case->GetRealVarVector())));
            cout << next_case->objective_function_value() << endl;
            bo_pso->SubmitEvaluatedCase(next_case);
        }

        // Get the first computed case
        auto next_case = bo_pso->GetCaseForEvaluation();
        cout << next_case->objective_function_value() << endl;
    }

    TEST_F(BO_PSOTest, TestFunctionSpherical) {
        test_case_ga_spherical_6r_->set_objective_function_value(- abs(Sphere(test_case_ga_spherical_6r_->GetRealVarVector())));
        Optimization::Optimizer *bo_pso = new BayesianOptimization::BO_PSO(settings_bo_ego_max_,
                                                                     test_case_ga_spherical_6r_,
                                                                     varcont_6r_,
                                                                     grid_5spot_,
                                                                     logger_
        );

        while (bo_pso->IsFinished() == Optimization::Optimizer::TerminationCondition::NOT_FINISHED) {
            auto next_case = bo_pso->GetCaseForEvaluation();
            next_case->set_objective_function_value(- abs(Sphere(next_case->GetRealVarVector())));
            next_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_DONE;
            bo_pso->SubmitEvaluatedCase(next_case);
        }
        auto best_case = bo_pso->GetTentativeBestCase();
//    EXPECT_NEAR(0.0, best_case->objective_function_value(), 0.1);
//    EXPECT_NEAR(0.0, best_case->GetRealVarVector()[0], 0.2);
//    EXPECT_NEAR(0.0, best_case->GetRealVarVector()[1], 0.2);
    }
    TEST_F(BO_PSOTest, TestFunctionRosenbrock) {
        test_case_ga_spherical_6r_->set_objective_function_value(abs(Rosenbrock(test_case_ga_spherical_6r_->GetRealVarVector())));
        Optimization::Optimizer *minimizer = new BayesianOptimization::BO_PSO(settings_bo_ego_min_, test_case_ga_spherical_6r_, varcont_6r_, grid_5spot_, logger_ );
        double i = 1;
        bool is_finished = false;
        while (!minimizer->IsFinished() && !is_finished) {
            auto next_case = minimizer->GetCaseForEvaluation();
            next_case->set_objective_function_value(Rosenbrock(next_case->GetRealVarVector()));
            if (next_case->objective_function_value() < 1)
                is_finished = true;
            minimizer->SubmitEvaluatedCase(next_case);
            i += 1;
        }
        auto best_case = minimizer->GetTentativeBestCase();
        cout << "This many generations were spent: " << i << endl;
        EXPECT_NEAR(0.0, best_case->objective_function_value(), 1);
        EXPECT_NEAR(1.0, best_case->GetRealVarVector()[0], 0.5);
        EXPECT_NEAR(1.0, best_case->GetRealVarVector()[1], 0.5);
    }


}

