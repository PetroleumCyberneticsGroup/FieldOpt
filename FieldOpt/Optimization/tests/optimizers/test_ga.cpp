
#include <gtest/gtest.h>
#include "optimizers/RGARDD.h"
#include "Optimization/optimizers/GeneticAlgorithm.h"
#include "Optimization/tests/test_resource_optimizer.h"
#include "Reservoir/tests/test_resource_grids.h"
#include "Optimization/tests/test_resource_test_functions.h"

using namespace TestResources::TestFunctions;
using namespace Optimization::Optimizers;

namespace {

class GeneticAlgorithmTest : public ::testing::Test,
                             public TestResources::TestResourceOptimizer,
                             public TestResources::TestResourceGrids {
 protected:
  GeneticAlgorithmTest() {
      base_ = base_case_;
  }
  virtual ~GeneticAlgorithmTest() {}
  virtual void SetUp() {}

  Optimization::Case *base_;

};

TEST_F(GeneticAlgorithmTest, Constructor) {
}

TEST_F(GeneticAlgorithmTest, TestFunctionSpherical) {
//    test_case_2r_->set_objective_function_value(abs(Sphere(test_case_2r_->GetRealVarVector())));
//    test_case_ga_spherical_30r_->set_objective_function_value(abs(Sphere(test_case_ga_spherical_30r_->GetRealVarVector())));
    test_case_ga_spherical_6r_->set_objective_function_value(abs(Sphere(test_case_ga_spherical_6r_->GetRealVarVector())));
    Optimization::Optimizer *minimizer = new RGARDD(settings_ga_min_,
                                                    test_case_ga_spherical_6r_, varcont_6r_, grid_5spot_);

    while (!minimizer->IsFinished()) {
        auto next_case = minimizer->GetCaseForEvaluation();
        next_case->set_objective_function_value(abs(Sphere(next_case->GetRealVarVector())));
        minimizer->SubmitEvaluatedCase(next_case);
    }
    auto best_case = minimizer->GetTentativeBestCase();
    EXPECT_NEAR(0.0, best_case->objective_function_value(), 0.1);
    EXPECT_NEAR(0.0, best_case->GetRealVarVector()[0], 0.1);
    EXPECT_NEAR(0.0, best_case->GetRealVarVector()[1], 0.1);
}

TEST_F(GeneticAlgorithmTest, TestFunctionRosenbrock) {
    test_case_ga_spherical_6r_->set_objective_function_value(abs(Rosenbrock(test_case_ga_spherical_6r_->GetRealVarVector())));
    Optimization::Optimizer *minimizer = new RGARDD(settings_ga_min_,
                                                    test_case_ga_spherical_6r_, varcont_6r_, grid_5spot_);

    while (!minimizer->IsFinished()) {
        auto next_case = minimizer->GetCaseForEvaluation();
        next_case->set_objective_function_value(Rosenbrock(next_case->GetRealVarVector()));
        minimizer->SubmitEvaluatedCase(next_case);
    }
    auto best_case = minimizer->GetTentativeBestCase();

    EXPECT_NEAR(0.0, best_case->objective_function_value(), 5);
    EXPECT_NEAR(1.0, best_case->GetRealVarVector()[0], 2.5);
    EXPECT_NEAR(1.0, best_case->GetRealVarVector()[1], 2.5);
}

}

