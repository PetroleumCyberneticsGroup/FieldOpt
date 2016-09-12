#include <gtest/gtest.h>
#include <QString>
#include <qvector.h>
#include "Simulation/results/eclresults.h"
#include "Settings/tests/test_resource_example_file_paths.hpp"

using namespace Simulation::Results;

namespace {

    class ECLResultsTest : public ::testing::Test {
    protected:
        ECLResultsTest() {
            results_ = new ECLResults();
        }

        virtual ~ECLResultsTest() { }

        virtual void SetUp() { }

        virtual void TearDown() {
            results_->DumpResults();
        }

        Results *results_;
    };

    TEST_F(ECLResultsTest, ReadSummary) {
        EXPECT_THROW(results_->ReadResults("a"), ResultFileNotFoundException);
        EXPECT_NO_THROW(results_->ReadResults(TestResources::ExampleFilePaths::ecl_base_horzwell));
    }

    TEST_F(ECLResultsTest, DumpingAndAvailability) {
        // Make results available
        results_->ReadResults(TestResources::ExampleFilePaths::ecl_base_horzwell);
        EXPECT_TRUE(results_->isAvailable());
        EXPECT_NO_THROW(results_->GetValue(ECLResults::Property::Time));

        // Make results unavailable
        results_->DumpResults();
        EXPECT_FALSE(results_->isAvailable());
        EXPECT_THROW(results_->GetValue(ECLResults::Property::Time), ResultsNotAvailableException);

        // Make results available again
        results_->ReadResults(TestResources::ExampleFilePaths::ecl_base_horzwell);
        EXPECT_TRUE(results_->isAvailable());
        EXPECT_NO_THROW(results_->GetValue(ECLResults::Property::Time));
    }

    TEST_F(ECLResultsTest, FieldVariables) {
        results_->ReadResults(TestResources::ExampleFilePaths::ecl_base_horzwell);
        EXPECT_FLOAT_EQ(187866.44, results_->GetValue(ECLResults::Property::CumulativeOilProduction));
        EXPECT_FLOAT_EQ(115870.73, results_->GetValue(ECLResults::Property::CumulativeOilProduction, 10));
        EXPECT_THROW(results_->GetValue(ECLResults::Property::CumulativeOilProduction, 30), std::runtime_error);

        std::vector<double> fopt_vec = results_->GetValueVector(ECLResults::Property::CumulativeOilProduction);
        EXPECT_EQ(21, fopt_vec.size());
        EXPECT_FLOAT_EQ(0, fopt_vec.front());
        EXPECT_FLOAT_EQ(187866.44, fopt_vec.back());
    }

    TEST_F(ECLResultsTest, MiscVariables) {
        results_->ReadResults(TestResources::ExampleFilePaths::ecl_base_horzwell);
        EXPECT_FLOAT_EQ(200, results_->GetValue(ECLResults::Property::Time));
        EXPECT_FLOAT_EQ(100, results_->GetValue(ECLResults::Property::Time, 10));
        EXPECT_THROW(results_->GetValue(ECLResults::Property::Time, 30), std::runtime_error);

        std::vector<double> time_vec = results_->GetValueVector(ECLResults::Property::Time);
        EXPECT_EQ(21, time_vec.size());
        EXPECT_EQ(0, time_vec.front());
        EXPECT_EQ(200, time_vec.back());
    }

    TEST_F(ECLResultsTest, WellVariables) {
        results_->ReadResults(TestResources::ExampleFilePaths::ecl_base_horzwell);
        EXPECT_FLOAT_EQ(1116.8876, results_->GetValue(ECLResults::Property::CumulativeWellWaterProduction, "PROD"));
        EXPECT_FLOAT_EQ(524.5061, results_->GetValue(ECLResults::Property::CumulativeWellWaterProduction, "PROD", 10));
    }

}
