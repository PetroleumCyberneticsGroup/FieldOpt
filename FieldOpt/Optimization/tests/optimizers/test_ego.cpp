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
#include "libgp/include/rprop.h"
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

TEST_F(EGOTest, GP5SpotExhaustiveTestData) {
    const int INITIAL_CASES = 5;
    const int ITERATIONS = 50;
    auto gen = get_random_generator();
    Optimization::Normalizer normalizer = Optimization::Normalizer();

    QFile file("/home/einar/PCG/FieldOpt/FieldOpt/Optimization/tests/optimizers/test_case_log_exhaustive.csv");
    if(!file.open(QIODevice::ReadOnly)) {
        cout << "Unable to open file." << endl;
    }
    QTextStream in(&file);
    vector<pair<Eigen::VectorXd, double>> cases;
    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(";");
        QStringList varstrings = fields[0].split(",");
        double ofv = fields[1].toDouble();
        Eigen::VectorXd vars = Eigen::VectorXd::Zero(2);
        for (int i = 0; i < varstrings.size() - 1; ++i) {
            vars(i) = varstrings[i].toDouble();
        }
        cases.push_back(pair<Eigen::VectorXd, double>(vars, ofv));
    }
    file.close();
    cout << "Number of cases: " << cases.size() << endl;

    libgp::GaussianProcess gp(2, "CovMatern5iso");
    Eigen::VectorXd params(2);
    params << 1, 1;
    gp.covf().set_loghyper(params);

    cout << "Training model with initial cases..." << endl;

    vector<int> initial_idxs = random_integers(gen, 0, cases.size()-1, INITIAL_CASES);

    // Train normalizer
    long double max_ofv = 0;
    for (int idx : initial_idxs) {
        if (cases[idx].second > max_ofv)
            max_ofv = cases[idx].second;
    }
    normalizer.set_midpoint(max_ofv);
    normalizer.set_steepness(1.0L/max_ofv);
    normalizer.set_max(1.0L);

    for (int idx : initial_idxs) {
        gp.add_pattern(cases[idx].first.data(), normalizer.normalize(cases[idx].second));
    }

    cout << "Optimizing hyperparameters..." << endl;
    libgp::RProp rprop;
    rprop.init();
    rprop.maximize(&gp, 50, 0);

    cout << "Initializing AF and AFOPT" << endl;
    Eigen::Vector2d lb, ub;
    lb << 1.0, 1.0;
    ub << 59.0, 59.0;
    auto af = BayesianOptimization::AcquisitionFunction();
    auto opt = BayesianOptimization::AFOptimizers::AFPSO(lb, ub);
    double target = normalizer.normalize(max_ofv);

    for (int iter = 0; iter < ITERATIONS; ++iter) {
        VectorXd next_point = opt.Optimize(&gp, af, target);
        next_point(0) = round(next_point(0));
        next_point(1) = round(next_point(1));
        double next_ofv;
        for (int i = 0; i < cases.size(); ++i) {
            if (cases[i].first(0) == next_point(0) && cases[i].first(1) == next_point(1))
                next_ofv = normalizer.normalize(cases[i].second);
        }
        if (next_ofv > target) {
            target = next_ofv;
            cout << "New best at i=" << iter << ": " << eigenvec_to_str(next_point) << " -> " << normalizer.denormalize(next_ofv) << endl;
        }
        gp.add_pattern(next_point.data(), next_ofv);
        libgp::RProp rprop;
        rprop.init();
        rprop.maximize(&gp, 50, 0);
    }

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

