/******************************************************************************
 *
 *
 *
 * Created: 04.11.2015 2015 by einar
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
#include "Model/wells/wellbore/trajectory.h"
#include "Model/wells/well_exceptions.h"

using namespace Model::Wells;

namespace {

class TrajectoryTest : public ::testing::Test {

protected:
    TrajectoryTest() {
        settings_ = new ::Utilities::Settings::Settings(driver_file_path_);
        variable_handler_ = new ::Model::Variables::VariableHandler(*settings_->model());
        variable_container_ = new ::Model::Variables::VariableContainer();
        prod_well_settings_ = settings_->model()->wells().first();
        prod_well_trajectory_ = new Wellbore::Trajectory(prod_well_settings_, variable_container_, variable_handler_);
    }
    virtual ~TrajectoryTest() {}
    virtual void SetUp() {}

    QString driver_file_path_ = "../../FieldOpt/GTest/Utilities/driver/driver.json";
    ::Utilities::Settings::Settings *settings_;
    ::Utilities::Settings::Model::Well prod_well_settings_;
    ::Model::Variables::VariableHandler *variable_handler_;
    ::Model::Variables::VariableContainer *variable_container_;
    ::Model::Wells::Wellbore::Trajectory *prod_well_trajectory_;
};

TEST_F(TrajectoryTest, Constructor) {
    EXPECT_TRUE(true);
}

TEST_F(TrajectoryTest, GetWellBlock) {
    EXPECT_NO_THROW(prod_well_trajectory_->GetWellBlock(0, 4, 2));
    EXPECT_THROW(prod_well_trajectory_->GetWellBlock(9, 9, 9), WellBlockNotFoundException);

    EXPECT_EQ(0, prod_well_trajectory_->GetWellBlock(0,4,2)->i());
    EXPECT_EQ(4, prod_well_trajectory_->GetWellBlock(0,4,2)->j());
    EXPECT_EQ(2, prod_well_trajectory_->GetWellBlock(0,4,2)->k());
    EXPECT_FALSE(prod_well_trajectory_->GetWellBlock(0,4,2)->HasCompletion());

    EXPECT_EQ(1, prod_well_trajectory_->GetWellBlock(1,4,2)->i());
    EXPECT_EQ(4, prod_well_trajectory_->GetWellBlock(1,4,2)->j());
    EXPECT_EQ(2, prod_well_trajectory_->GetWellBlock(1,4,2)->k());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlock(1,4,2)->HasCompletion());

    EXPECT_EQ(2, prod_well_trajectory_->GetWellBlock(2,4,2)->i());
    EXPECT_EQ(4, prod_well_trajectory_->GetWellBlock(2,4,2)->j());
    EXPECT_EQ(2, prod_well_trajectory_->GetWellBlock(2,4,2)->k());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlock(2,4,2)->HasCompletion());

    EXPECT_EQ(3, prod_well_trajectory_->GetWellBlock(3,4,2)->i());
    EXPECT_EQ(4, prod_well_trajectory_->GetWellBlock(3,4,2)->j());
    EXPECT_EQ(2, prod_well_trajectory_->GetWellBlock(3,4,2)->k());
    EXPECT_FALSE(prod_well_trajectory_->GetWellBlock(3,4,2)->HasCompletion());
}

TEST_F(TrajectoryTest, AllWellBlocks) {
    EXPECT_EQ(4, prod_well_trajectory_->GetWellBlocks()->size());
    EXPECT_FALSE(prod_well_trajectory_->GetWellBlocks()->at(0)->HasCompletion());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlocks()->at(1)->HasCompletion());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlocks()->at(2)->HasCompletion());
    EXPECT_FALSE(prod_well_trajectory_->GetWellBlocks()->at(3)->HasCompletion());
}

TEST_F(TrajectoryTest, Completions) {
    EXPECT_EQ(::Model::Wells::Wellbore::Completions::Completion::CompletionType::Perforation,
              prod_well_trajectory_->GetWellBlock(1,4,2)->GetCompletion()->type());
    EXPECT_FALSE(prod_well_trajectory_->GetWellBlock(0,4,2)->HasPerforation());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlock(1,4,2)->HasPerforation());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlock(2,4,2)->HasPerforation());
    EXPECT_FALSE(prod_well_trajectory_->GetWellBlock(3,4,2)->HasPerforation());
    EXPECT_FLOAT_EQ(1.0, prod_well_trajectory_->GetWellBlock(1,4,2)->GetPerforation()->transmissibility_factor());
    EXPECT_FLOAT_EQ(1.0, prod_well_trajectory_->GetWellBlock(2,4,2)->GetPerforation()->transmissibility_factor());

    EXPECT_THROW(prod_well_trajectory_->GetWellBlock(0,4,2)->GetPerforation(), PerforationNotDefinedForWellBlockException);
}


}

