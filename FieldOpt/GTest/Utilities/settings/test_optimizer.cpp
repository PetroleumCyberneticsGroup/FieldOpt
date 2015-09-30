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

#include "Utilities/settings/settings.h"
#include "Utilities/settings/optimizer.h"

using namespace Utilities::Settings;

namespace {

class OptimizerSettingsTest : public ::testing::Test {
protected:
    OptimizerSettingsTest()
        : settings_(driver_file_path_)
    {
    }
    virtual ~OptimizerSettingsTest() {}

    virtual void SetUp() {}
    virtual void TearDown() {}

    QString driver_file_path_ = "../../FieldOpt/GTest/Utilities/driver/driver.json";
    Settings settings_;
};

TEST_F(OptimizerSettingsTest, Type) {
    EXPECT_EQ(Optimizer::OptimizerType::Compass, settings_.optimizer()->type());
}

TEST_F(OptimizerSettingsTest, Parameters) {
    Optimizer::Parameters params = settings_.optimizer()->parameters();
    EXPECT_EQ(10, params.max_evaluations);
    EXPECT_FLOAT_EQ(50.0, params.initial_step_length);
    EXPECT_FLOAT_EQ(1.0, params.minimum_step_length);
}

TEST_F(OptimizerSettingsTest, Objective) {
    Optimizer::Objective obj = settings_.optimizer()->objective();
    EXPECT_EQ(Optimizer::ObjectiveType::Expression, obj.type);
    EXPECT_STREQ("CUMOIL - 0.5*CUMWAT", obj.expression.toLatin1().constData());
}

TEST_F(OptimizerSettingsTest, ProducerConstraint) {
    Optimizer::Constraint producerConstraint = settings_.optimizer()->constraints()->first();
    EXPECT_EQ(Optimizer::ConstraintType::BHP, producerConstraint.type);
    EXPECT_STREQ("PROD", producerConstraint.well.toLatin1().constData());
    EXPECT_FLOAT_EQ(3000.0, producerConstraint.max);
    EXPECT_FLOAT_EQ(1000.0, producerConstraint.min);
}

TEST_F(OptimizerSettingsTest, InjectorConstraint) {
    Optimizer::Constraint injectorConstraint = settings_.optimizer()->constraints()->last();
    EXPECT_EQ(Optimizer::ConstraintType::SplinePoints, injectorConstraint.type);
    EXPECT_STREQ("INJ", injectorConstraint.well.toLatin1().constData());

    EXPECT_EQ(Optimizer::ConstraintWellSplinePointsType::MaxMin, injectorConstraint.spline_points_type);
    EXPECT_EQ(2, injectorConstraint.spline_points_limits.size());
    EXPECT_FLOAT_EQ(40.0, injectorConstraint.spline_points_limits.first().max.z);
}

}
