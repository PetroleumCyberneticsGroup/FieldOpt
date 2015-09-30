/******************************************************************************
 *
 * test_model.cpp
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
#include "Utilities/settings/model.h"

using namespace Utilities::Settings;

namespace {

class ModelSettingsTest : public ::testing::Test {
protected:
    ModelSettingsTest()
        : settings_(driver_file_path_)
    {
    }
    virtual ~ModelSettingsTest() {}

    virtual void SetUp() {}
    virtual void TearDown() {}

    QString driver_file_path_ = "../../FieldOpt/GTest/Utilities/driver/driver.json";
    Settings settings_;
};

TEST_F(ModelSettingsTest, Reservoir) {
    EXPECT_EQ(settings_.model()->reservoir().type, Model::ReservoirGridSourceType::ECLIPSE);
    EXPECT_STREQ("../../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID", settings_.model()->reservoir().path.toLatin1().constData());
}

TEST_F(ModelSettingsTest, ProducerWell) {
    Model::Well producer = settings_.model()->wells().first();
    EXPECT_STREQ("PROD", producer.name.toLatin1().constData());
    EXPECT_EQ(Model::WellType::Producer, producer.type);
    EXPECT_EQ(Model::WellDefinitionType::WellBlocks, producer.definition_type);
    EXPECT_EQ(Model::WellControlType::BHPControl, producer.control);
    EXPECT_FLOAT_EQ(2000.0, producer.bhp);

    EXPECT_EQ(0, producer.heel.i);
    EXPECT_EQ(0, producer.heel.j);
    EXPECT_EQ(0, producer.heel.k);
    EXPECT_EQ(3, producer.well_blocks[3].i);
    EXPECT_EQ(4, producer.well_blocks.size());

    EXPECT_EQ(1, producer.variables.size());
    EXPECT_EQ(Model::WellVariableType::BHP, producer.variables.first().type);
    EXPECT_EQ(3, producer.variables.first().time_steps.size());
}

TEST_F(ModelSettingsTest, InjectorWell) {
    Model::Well injector = settings_.model()->wells().last();
    EXPECT_STREQ("INJ", injector.name.toLatin1().constData());
    EXPECT_EQ(Model::WellType::Injector, injector.type);
    EXPECT_EQ(Model::WellControlType::RateControl, injector.control);
    EXPECT_FLOAT_EQ(1200.0, injector.rate);

    EXPECT_EQ(2, injector.heel.i);
    EXPECT_EQ(0, injector.heel.j);
    EXPECT_EQ(2, injector.heel.k);

    EXPECT_EQ(Model::WellDefinitionType::WellSpline, injector.definition_type);
    EXPECT_EQ(2, injector.spline_points.size());
    EXPECT_FLOAT_EQ(40.0, injector.spline_points[1].y);

    EXPECT_EQ(1, injector.variables.size());
    EXPECT_EQ(Model::WellVariableType::SplinePoints, injector.variables.first().type);
    EXPECT_EQ(1, injector.variables.first().variable_spline_point_indices.last());
    EXPECT_EQ(1, injector.variables.first().time_steps.size());
}

}
