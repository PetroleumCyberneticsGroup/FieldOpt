#include <gtest/gtest.h>
#include "Optimization/optimizers/trust_region_search.h"
#include "Optimization/tests/test_resource_optimizer.h"
#include "Reservoir/tests/test_resource_grids.h"

namespace {

    class TrustRegionSearchTest : public ::testing::Test, TestResources::TestResourceOptimizer, TestResources::TestResourceGrids {
    protected:
        TrustRegionSearchTest() {
            trust_region_search_ = new ::Optimization::Optimizers::TrustRegionSearch(settings_optimizer_, base_case_, model_->variables(), grid_5spot_);
        }
        virtual ~TrustRegionSearchTest() {}
        virtual void SetUp() {}

        Optimization::Optimizer *trust_region_search_;
    };

    TEST_F(TrustRegionSearchTest, Constructor) {
        EXPECT_FLOAT_EQ(1000.0, trust_region_search_->GetTentativeBestCase()->objective_function_value());
    }

    TEST_F(TrustRegionSearchTest, BogoTest) {
        EXPECT_TRUE(true);
    }

    TEST_F(TrustRegionSearchTest, PointCaseConversionTest) {
        Optimization::Case* a = trust_region_search_->GetTentativeBestCase();
        Eigen::VectorXd vec = a->GetRealVarVector();
        Optimization::Case* b = PolyModel::CaseFromPoint(a->GetRealVarVector(), a);
        EXPECT_TRUE(a->Equals(b));
    }

    TEST_F(TrustRegionSearchTest, CompleteSetOfPointsTest) {
        PolyModel poly_model = PolyModel(trust_region_search_->GetTentativeBestCase(), 1);
        std::cout << "Created Polymodel" << std::endl;
        poly_model.complete_points();
        std::cout << "Completed set of points" << std::endl;
        EXPECT_FALSE(poly_model.isModelReady());
    }

    TEST_F(TrustRegionSearchTest, OneIterationTest) {
        Optimization::Case *tentative_best_0 = trust_region_search_->GetTentativeBestCase();
        for (int i = 0; i < 220; ++i) {
            Optimization::Case *new_case = trust_region_search_->GetCaseForEvaluation();
            new_case->set_objective_function_value((i%3)*700);
            trust_region_search_->SubmitEvaluatedCase(new_case);
        }

        Optimization::Case *tentative_best_1 = trust_region_search_->GetTentativeBestCase();
        EXPECT_TRUE(tentative_best_1->objective_function_value() == tentative_best_0->objective_function_value());
    }
}
