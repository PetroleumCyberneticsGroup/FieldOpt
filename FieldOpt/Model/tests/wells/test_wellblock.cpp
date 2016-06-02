#include <gtest/gtest.h>

#include "Model/wells/wellbore/wellblock.h"

using namespace Model::Wells::Wellbore;

namespace {

class WellBlockTest : public ::testing::Test {
protected:
    WellBlockTest() {
        well_block_ = new WellBlock(1, 2, 3, 1);
    }
    virtual ~WellBlockTest() {}

    WellBlock *well_block_;
};

TEST_F(WellBlockTest, Constructor) {
    EXPECT_TRUE(true);
}


TEST_F(WellBlockTest, Values) {
    EXPECT_EQ(1, well_block_->id());
    EXPECT_EQ(1, well_block_->i());
    EXPECT_EQ(2, well_block_->j());
    EXPECT_EQ(3, well_block_->k());
    EXPECT_FALSE(well_block_->HasCompletion());
}



TEST_F(WellBlockTest, Modifiability) {
    well_block_->setI(4);
    well_block_->setJ(5);
    well_block_->setK(6);
    EXPECT_EQ(4, well_block_->i());
    EXPECT_EQ(5, well_block_->j());
    EXPECT_EQ(6, well_block_->k());
}


}

