/******************************************************************************
 *
 *
 *
 * Created: 10.11.2015 2015 by einar
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

#include "GTest/Model/test_fixture_model_base.h"
#include "Model/wells/well.h"

namespace {

class WellTest : public ModelBaseTest {
protected:
    WellTest() {
        producer_well_ = new ::Model::Wells::Well(*settings_->model(), 0, variable_container_, variable_handler_, model_->reservoir());
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
    EXPECT_EQ(::Utilities::Settings::Model::PreferedPhase::Oil, producer_well_->prefered_phase());
}

TEST_F(WellTest, Trajectory) {
    EXPECT_NO_THROW(producer_well_->trajectory());
    EXPECT_EQ(4, producer_well_->trajectory()->GetWellBlocks()->size());
}

TEST_F(WellTest, Heel) {
    EXPECT_EQ(1, producer_well_->heel_i());
    EXPECT_EQ(1, producer_well_->heel_j());
    EXPECT_EQ(1, producer_well_->heel_k());
}

TEST_F(WellTest, Controls) {
    EXPECT_NO_THROW(producer_well_->controls());
    EXPECT_GE(producer_well_->controls()->size(), 1);
}

}
