/******************************************************************************
 *
 * test_optimizer.cpp
 *
 * Created: 30.09.2015 2015 by einar
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
#include <QString>

#include "Utilities/tests/test_resource_settings.h"

using namespace Utilities::Settings;

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

    TEST_F(OptimizerSettingsTest, ProducerConstraint) {
        Optimizer::Constraint producerConstraint = settings_optimizer_->constraints()->first();
        EXPECT_EQ(Optimizer::ConstraintType::BHP, producerConstraint.type);
        EXPECT_STREQ("PROD", producerConstraint.well.toLatin1().constData());
        EXPECT_FLOAT_EQ(3000.0, producerConstraint.max);
        EXPECT_FLOAT_EQ(1000.0, producerConstraint.min);
    }

}
