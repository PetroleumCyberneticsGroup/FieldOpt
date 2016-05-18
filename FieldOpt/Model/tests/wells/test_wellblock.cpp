/******************************************************************************
 *
 *
 *
 * Created: 29.10.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

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

