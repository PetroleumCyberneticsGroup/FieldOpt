/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <gtest/gtest.h>
#include <QString>

#include "Settings/tests/test_resource_settings.hpp"

using namespace Settings;

namespace {

class OptimizerSettingsTest : public ::testing::Test, public TestResources::TestResourceSettings {
 protected:
  OptimizerSettingsTest()
  { }
  virtual ~OptimizerSettingsTest() {}

  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(OptimizerSettingsTest, Type) {
    EXPECT_EQ(Optimizer::OptimizerType::Compass, settings_optimizer_->type());
}

TEST_F(OptimizerSettingsTest, Mode) {
    EXPECT_EQ(Optimizer::OptimizerMode::Maximize, settings_optimizer_->mode());
}

TEST_F(OptimizerSettingsTest, Parameters) {
    Optimizer::Parameters params = settings_optimizer_->parameters();
    EXPECT_EQ(10, params.max_evaluations);
    EXPECT_FLOAT_EQ(50.0, params.initial_step_length);
    EXPECT_FLOAT_EQ(1.0, params.minimum_step_length);
}

TEST_F(OptimizerSettingsTest, Objective) {
    Optimizer::Objective obj = settings_optimizer_->objective();
    EXPECT_EQ(Optimizer::ObjectiveType::WeightedSum, obj.type);
    EXPECT_EQ(2, obj.weighted_sum.size());

    EXPECT_STREQ("CumulativeOilProduction", obj.weighted_sum.at(0).property.toLatin1().constData());
    EXPECT_FLOAT_EQ(1.0, obj.weighted_sum.at(0).coefficient);
    EXPECT_EQ(-1, obj.weighted_sum.at(0).time_step);
    EXPECT_FALSE(obj.weighted_sum.at(0).is_well_prop);

    EXPECT_STREQ("CumulativeWellWaterProduction", obj.weighted_sum.at(1).property.toLatin1().constData());
    EXPECT_FLOAT_EQ(-0.2, obj.weighted_sum.at(1).coefficient);
    EXPECT_EQ(10, obj.weighted_sum.at(1).time_step);
    EXPECT_TRUE(obj.weighted_sum.at(1).is_well_prop);
    EXPECT_STREQ("PROD", obj.weighted_sum.at(1).well.toLatin1().constData());
}


TEST_F(OptimizerSettingsTest, Constraints) {
    EXPECT_EQ(6, settings_optimizer_->constraints().length());
    EXPECT_EQ(Optimizer::ConstraintType::BHP, settings_optimizer_->constraints()[0].type);
    EXPECT_EQ(Optimizer::ConstraintType::WellSplineLength, settings_optimizer_->constraints()[1].type);
    EXPECT_EQ(Optimizer::ConstraintType::Rate, settings_optimizer_->constraints()[2].type);
    EXPECT_EQ(Optimizer::ConstraintType::WellSplineInterwellDistance, settings_optimizer_->constraints()[3].type);
    EXPECT_EQ(Optimizer::ConstraintType::CombinedWellSplineLengthInterwellDistance, settings_optimizer_->constraints()[4].type);
}

TEST_F(OptimizerSettingsTest, BHPConstraint) {
    auto constr = settings_optimizer_->constraints()[0];
    EXPECT_STREQ("PROD", constr.well.toLatin1().constData());
    EXPECT_FLOAT_EQ(300.0, constr.max);
    EXPECT_FLOAT_EQ(90.0, constr.min);
}

TEST_F(OptimizerSettingsTest, WellSplineLengthConstraint) {
    auto constr = settings_optimizer_->constraints()[1];
    EXPECT_STREQ("INJ", constr.well.toLatin1().constData());
    EXPECT_FLOAT_EQ(400, constr.min_length);
    EXPECT_FLOAT_EQ(1200, constr.max_length);
    EXPECT_FLOAT_EQ(400, constr.min);
    EXPECT_FLOAT_EQ(1200, constr.max);
}


TEST_F(OptimizerSettingsTest, RateConstraint) {
    auto constr = settings_optimizer_->constraints()[2];
    EXPECT_STREQ("INJ", constr.well.toLatin1().constData());
    EXPECT_FLOAT_EQ(1200, constr.min);
    EXPECT_FLOAT_EQ(1400, constr.max);
}


TEST_F(OptimizerSettingsTest, InterwellDistanceConstraint) {
    auto constr = settings_optimizer_->constraints()[3];
    EXPECT_EQ(2, constr.wells.length());
    EXPECT_STREQ("TESTW", constr.wells[1].toLatin1().constData());
    EXPECT_FLOAT_EQ(100, constr.min);
    EXPECT_FLOAT_EQ(100, constr.min_distance);
}

TEST_F(OptimizerSettingsTest, CombinedSplineLengthInterwellDistanceConstraint) {
    auto constr = settings_optimizer_->constraints()[4];
    EXPECT_EQ(2, constr.wells.length());
    EXPECT_STREQ("TESTW", constr.wells[1].toLatin1().constData());
    EXPECT_FLOAT_EQ(100, constr.min_distance);
    EXPECT_FLOAT_EQ(400, constr.min_length);
    EXPECT_FLOAT_EQ(1200, constr.max_length);
    EXPECT_EQ(50, constr.max_iterations);
}

TEST_F(OptimizerSettingsTest, CombinedSplineLengthInterwellDistanceConstraintReservoirBoundary) {
    auto constr = settings_optimizer_->constraints()[4];
    EXPECT_EQ(2, constr.wells.length());
    EXPECT_STREQ("TESTW", constr.wells[1].toLatin1().constData());
    EXPECT_FLOAT_EQ(100, constr.min_distance);
    EXPECT_FLOAT_EQ(400, constr.min_length);
    EXPECT_FLOAT_EQ(1200, constr.max_length);
    EXPECT_EQ(50, constr.max_iterations);
}
}
