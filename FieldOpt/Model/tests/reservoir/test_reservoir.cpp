#include <gtest/gtest.h>
#include "Model/reservoir/reservoir.h"
#include "Utilities/tests/test_resource_settings.h"

using namespace Model::Reservoir;

namespace {

class ReservoirTest : public ::testing::Test, public TestResources::TestResourceSettings {
protected:
    ReservoirTest() {
        reservoir_ = new Reservoir(settings_model_->reservoir());
    }

    virtual ~ReservoirTest() {}

    virtual void SetUp() {}

    virtual void TearDown() {}

    Reservoir *reservoir_;
};

TEST_F(ReservoirTest, Constructor) {
    EXPECT_TRUE(true);
}

TEST_F(ReservoirTest, CheckDimensions) {
    Grid::Grid::Dims dims = reservoir_->grid()->Dimensions();
    EXPECT_EQ(dims.nx, 60);
    EXPECT_EQ(dims.ny, 60);
    EXPECT_EQ(dims.nz, 1);
}

}
