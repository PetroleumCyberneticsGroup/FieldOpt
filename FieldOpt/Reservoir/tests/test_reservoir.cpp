#include <gtest/gtest.h>
#include "Reservoir/reservoir.h"
#include "Utilities/tests/test_resource_settings.hpp"

using namespace Reservoir;

namespace {

class ReservoirTest : public ::testing::Test, public TestResources::TestResourceSettings {
protected:
    ReservoirTest() {
        reservoir_ = new Reservoir::Reservoir(settings_model_->reservoir());
    }

    virtual ~ReservoirTest() {}

    virtual void SetUp() {}

    virtual void TearDown() {}

    Reservoir::Reservoir *reservoir_;
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
