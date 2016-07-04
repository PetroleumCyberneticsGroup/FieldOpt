#include <gtest/gtest.h>
#include "Utilities/tests/test_resource_example_file_paths.h"
#include "Simulation/results/adgprsresults.h"

namespace {

    class AdgprsResultsTest : public ::testing::Test {
    protected:
        AdgprsResultsTest()
        {
            results_ = new Simulation::Results::AdgprsResults();
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
        EXPECT_EQ(8, results_->GetValueVector(Simulation::Results::Results::Property::Time).size());
    }

    TEST_F(AdgprsResultsTest, FOPT) {
        EXPECT_EQ(8, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeOilProduction).size());
        EXPECT_FLOAT_EQ(0.0, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeOilProduction).front());
        EXPECT_FLOAT_EQ(180388.48, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeOilProduction).back());
    }

    TEST_F(AdgprsResultsTest, FWPT) {
        EXPECT_EQ(8, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeWaterProduction).size());
        EXPECT_FLOAT_EQ(0.0, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeWaterProduction).front());
        EXPECT_FLOAT_EQ(316942.25, results_->GetValueVector(Simulation::Results::Results::Property::CumulativeWaterProduction).back());
    }

    TEST_F(AdgprsResultsTest, FGPT) {
        EXPECT_THROW(results_->GetValueVector(Simulation::Results::Results::Property::CumulativeGasProduction), std::runtime_error);
    }

}
