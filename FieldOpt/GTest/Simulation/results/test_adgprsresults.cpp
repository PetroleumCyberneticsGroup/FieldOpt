#include "test_fixture_adgprs.h"
#include "Simulation/results/adgprsresults.h"

namespace {

class AdgprsResultsTest : public AdgprsTestFixture {
protected:
    AdgprsResultsTest()
    {
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

}
