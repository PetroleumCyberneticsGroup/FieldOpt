#include <gtest/gtest.h>
#include "Optimization/optimizers/compass_search.h"
#include "Optimization/tests/test_resource_optimizer.h"
#include "Reservoir/tests/test_resource_grids.h"

namespace {

    class CompassSearchTest : public ::testing::Test, TestResources::TestResourceOptimizer, TestResources::TestResourceGrids {
    protected:
        CompassSearchTest() {
            compass_search_ = new ::Optimization::Optimizers::CompassSearch(settings_optimizer_, base_case_, model_->variables(), grid_5spot_);
            base_ = base_case_;
        }
        virtual ~CompassSearchTest() {}
        virtual void SetUp() {}

        Optimization::Optimizer *compass_search_;
        Optimization::Case *base_;
    };

    TEST_F(CompassSearchTest, Constructor) {
        EXPECT_FLOAT_EQ(1000.0, compass_search_->GetTentativeBestCase()->objective_function_value());
    }

    TEST_F(CompassSearchTest, GetNewCases) {
        // These four cases should change the values of the two first int vars, +50 then -50
        Optimization::Case *new_case_1 = compass_search_->GetCaseForEvaluation();
        Optimization::Case *new_case_2 = compass_search_->GetCaseForEvaluation();
        Optimization::Case *new_case_3 = compass_search_->GetCaseForEvaluation();
        Optimization::Case *new_case_4 = compass_search_->GetCaseForEvaluation();
        EXPECT_FALSE(new_case_1->id() == new_case_2->id());
        EXPECT_FALSE(new_case_3->id() == new_case_4->id());

        EXPECT_EQ(new_case_1->GetIntegerVarVector()[0], base_->GetIntegerVarVector()[0] + 50);
        EXPECT_EQ(new_case_2->GetIntegerVarVector()[0], base_->GetIntegerVarVector()[0] - 50);
        EXPECT_EQ(new_case_3->GetIntegerVarVector()[1], base_->GetIntegerVarVector()[1] + 50);
        EXPECT_EQ(new_case_4->GetIntegerVarVector()[1], base_->GetIntegerVarVector()[1] - 50);
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
