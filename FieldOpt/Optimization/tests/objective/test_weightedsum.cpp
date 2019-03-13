#include <gtest/gtest.h>
#include <QString>
#include "Optimization/objective/weightedsum.h"
#include "Simulation/tests/test_resource_results.h"
#include "Settings/tests/test_resource_settings.hpp"
#include "Model/tests/test_resource_model.h"


using namespace Optimization::Objective;
using namespace Simulation::Results;
using namespace Settings;

namespace {

class WeightedSumTest : public ::testing::Test, public TestResources::TestResourceResults,
        public TestResources::TestResourceModel {
protected:
    WeightedSumTest() { }

    virtual ~WeightedSumTest() { }

    virtual void SetUp() { }

    virtual void TearDown() { }

};

TEST_F(WeightedSumTest, Value) {
    auto *obj = new WeightedSum(settings_optimizer_, results_ecl_horzwell_, model_);
    float wwpt = results_ecl_horzwell_->GetValue(Results::Property::CumulativeWellWaterProduction, "PROD", 10);
    float fopt = results_ecl_horzwell_->GetValue(Results::Property::CumulativeOilProduction);
    EXPECT_FLOAT_EQ(100.0, results_ecl_horzwell_->GetValue(Results::Property::Time, 10));
    EXPECT_FLOAT_EQ(wwpt, 524.5061);
    EXPECT_FLOAT_EQ(fopt, 187866.44);
    EXPECT_FLOAT_EQ(fopt - 0.2*wwpt, obj->value());
}

}
