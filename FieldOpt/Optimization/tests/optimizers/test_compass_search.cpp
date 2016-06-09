#include <gtest/gtest.h>
#include "Optimization/optimizers/compass_search.h"
#include "Optimization/tests/test_resource_optimizer.h"

namespace {

class CompassSearchTest : public ::testing::Test, TestResources::TestResourceOptimizer {
protected:
    CompassSearchTest() {
        compass_search_ = new ::Optimization::Optimizers::CompassSearch(settings_optimizer_, base_case_, model_->variables());
    }
    virtual ~CompassSearchTest() {}
    virtual void SetUp() {}

    Optimization::Optimizer *compass_search_;
};

TEST_F(CompassSearchTest, Constructor) {
    EXPECT_FLOAT_EQ(1000.0, compass_search_->GetTentativeBestCase()->objective_function_value());
}

TEST_F(CompassSearchTest, GetNewCases) {
    Optimization::Case *new_case_1 = compass_search_->GetCaseForEvaluation();
    Optimization::Case *new_case_2 = compass_search_->GetCaseForEvaluation();
    Optimization::Case *new_case_3 = compass_search_->GetCaseForEvaluation();
    Optimization::Case *new_case_4 = compass_search_->GetCaseForEvaluation();
    EXPECT_FALSE(new_case_1->id() == new_case_2->id());
    EXPECT_FALSE(new_case_3->id() == new_case_4->id());
}

TEST_F(CompassSearchTest, Pseudoiterations) {
    Optimization::Case *tentative_best_0 = compass_search_->GetTentativeBestCase();
    for (int i = 0; i < 100; ++i) {
        Optimization::Case *new_case = compass_search_->GetCaseForEvaluation();
        new_case->set_objective_function_value((i%3)*700);
        compass_search_->SubmitEvaluatedCase(new_case);
    }
    Optimization::Case *tentative_best_1 = compass_search_->GetTentativeBestCase();
    EXPECT_TRUE(tentative_best_1->objective_function_value() > tentative_best_0->objective_function_value());

    for (int i = 100; i < 150; ++i) {
        Optimization::Case *new_case = compass_search_->GetCaseForEvaluation();
        new_case->set_objective_function_value((i%3)*800);
        compass_search_->SubmitEvaluatedCase(new_case);
    }
    Optimization::Case *tentative_best_2 = compass_search_->GetTentativeBestCase();
    EXPECT_TRUE(tentative_best_2->objective_function_value() > tentative_best_1->objective_function_value());
}

}
