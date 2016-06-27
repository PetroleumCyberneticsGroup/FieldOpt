#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"

namespace {

class WellTest : public ::testing::Test, public TestResources::TestResourceModel{
protected:
    WellTest() {
        producer_well_ = model_->wells()->first();
    }

    ::Model::Wells::Well *producer_well_;
};

TEST_F(WellTest, Constructor) {
    EXPECT_TRUE(true);
}

TEST_F(WellTest, BasicFields) {
    EXPECT_STREQ("PROD", producer_well_->name().toLatin1().constData());
    EXPECT_FLOAT_EQ(0.75, producer_well_->wellbore_radius());
    EXPECT_EQ(::Utilities::Settings::Model::WellType::Producer, producer_well_->type());
    EXPECT_EQ(::Utilities::Settings::Model::PreferredPhase::Oil, producer_well_->preferred_phase());
}

TEST_F(WellTest, Trajectory) {
    EXPECT_NO_THROW(producer_well_->trajectory());
    EXPECT_EQ(4, producer_well_->trajectory()->GetWellBlocks()->size());
}

TEST_F(WellTest, Heel) {
    EXPECT_EQ(1, producer_well_->heel_i());
    EXPECT_EQ(4, producer_well_->heel_j());
    EXPECT_EQ(1, producer_well_->heel_k());
}

TEST_F(WellTest, Controls) {
    EXPECT_NO_THROW(producer_well_->controls());
    EXPECT_GE(producer_well_->controls()->size(), 1);
}

}
