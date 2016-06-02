#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"
#include "Simulation/results/adgprsresults.h"

namespace {

class AdgprsResultsTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    AdgprsResultsTest()
    {
        results_ = new Simulation::Results::AdgprsResults(model_);
        results_->ReadResults(TestResources::ExampleFilePaths::gprs_base_5spot_);
    }
    virtual ~AdgprsResultsTest() {}
    virtual void SetUp() {}
    Simulation::Results::AdgprsResults *results_;
};

TEST_F(AdgprsResultsTest, ReadFile) {
    EXPECT_TRUE(true);
}

TEST_F(AdgprsResultsTest, Time) {
    EXPECT_FLOAT_EQ(0.0, results_->GetValue(Simulation::Results::Results::Property::Time, 0));
    EXPECT_FLOAT_EQ(100.0, results_->GetValue(Simulation::Results::Results::Property::Time));
    EXPECT_EQ(8, results_->GetValueVector(Simulation::Results::Results::Property::Time).length());
}

TEST_F(AdgprsResultsTest, FOPT) {
    EXPECT_EQ(8, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeOilProduction).length());
    EXPECT_FLOAT_EQ(0.0, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeOilProduction).first());
    EXPECT_FLOAT_EQ(359464.72, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeOilProduction).last());
}

TEST_F(AdgprsResultsTest, FWPT) {
    EXPECT_EQ(8, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeWaterProduction).length());
    EXPECT_FLOAT_EQ(0.0, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeWaterProduction).first());
    EXPECT_FLOAT_EQ(347556.34, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeWaterProduction).last());
}

TEST_F(AdgprsResultsTest, FGPT) {
    EXPECT_EQ(8, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeGasProduction).length());
    EXPECT_FLOAT_EQ(0.0, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeGasProduction).first());
    EXPECT_FLOAT_EQ(0.0, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeGasProduction).last());
}

}
