#include "test_fixture_adgprs.h"
#include "Simulation/results/adgprsresults.h"

namespace {

class AdgprsResultsTest : public AdgprsTestFixture {
protected:
    AdgprsResultsTest() {
        results_ = new Simulation::Results::AdgprsResults(model_);
        results_->ReadResults(base_summary_path_);
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
    EXPECT_FLOAT_EQ(2920.0, results_->GetValue(Simulation::Results::Results::Property::Time));
}

TEST_F(AdgprsResultsTest, Properties) {
    // Check the calculation of the field value
    double calculated_fopt = 0.0;
    for (int i = 1; i < 5; ++i) { // The four last wells are producers
        calculated_fopt += results_->GetValue(i, Simulation::Results::Results::Property::CumulativeWellOilProduction);
    }
    EXPECT_FLOAT_EQ(calculated_fopt, results_->GetValue(Simulation::Results::Results::CumulativeOilProduction));
}

}
