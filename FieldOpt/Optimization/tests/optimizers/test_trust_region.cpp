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

TEST_F(TrustRegionSearchTest, MakeModel) {
    EXPECT_TRUE(true);
}

}
