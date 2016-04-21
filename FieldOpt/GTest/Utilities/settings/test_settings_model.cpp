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
        : settings_(driver_file_path_, output_directory_)
    {
    }
    virtual ~ModelSettingsTest() {}

    virtual void SetUp() {}
    virtual void TearDown() {}

    QString driver_file_path_ = "../../FieldOpt/GTest/Utilities/driver/driver.json";
    QString output_directory_ = "/home/einar/Documents/GitHub/PCG/fieldopt_output";
    Settings settings_;
};

TEST_F(ModelSettingsTest, Reservoir) {
    EXPECT_EQ(settings_.model()->reservoir().type, Model::ReservoirGridSourceType::ECLIPSE);
    EXPECT_STREQ("../../examples/ADGPRS/5spot/ECL_5SPOT.EGRID", settings_.model()->reservoir().path.toLatin1().constData());
}

TEST_F(ModelSettingsTest, ControlTimes) {
    EXPECT_EQ(4, settings_.model()->control_times().size());
    EXPECT_EQ(365, settings_.model()->control_times().last());
}

TEST_F(ModelSettingsTest, ProducerWell) {
    Model::Well producer = settings_.model()->wells().first();
    EXPECT_STREQ("PROD", producer.name.toLatin1().constData());
    EXPECT_EQ(Model::WellType::Producer, producer.type);
    EXPECT_EQ(Model::WellDefinitionType::WellBlocks, producer.definition_type);
    EXPECT_EQ(Model::ControlMode::BHPControl, producer.controls.first().control_mode);
    EXPECT_FLOAT_EQ(2000.0, producer.controls.first().bhp);
    EXPECT_EQ(0, producer.controls.first().time_step);
    EXPECT_EQ(Model::WellState::WellOpen, producer.controls.first().state);
    EXPECT_EQ(Model::PreferedPhase::Oil, producer.prefered_phase);
    EXPECT_FLOAT_EQ(0.75, producer.wellbore_radius);
    EXPECT_EQ(Model::Direction::X, producer.direction);

    EXPECT_EQ(1, producer.heel.i);
    EXPECT_EQ(1, producer.heel.j);
    EXPECT_EQ(1, producer.heel.k);
    EXPECT_EQ(4, producer.well_blocks[3].position.i);
    EXPECT_EQ(4, producer.well_blocks.size());
}

TEST_F(ModelSettingsTest, ProducerCompletions) {
    Model::Well producer = settings_.model()->wells().first();
    EXPECT_EQ(Model::WellCompletionType::Perforation, producer.completions.first().type);
    EXPECT_EQ(4, producer.completions.first().well_block.position.j);
    EXPECT_EQ(0, producer.completions[0].id);
    EXPECT_EQ(1, producer.completions[1].id);
}

TEST_F(ModelSettingsTest, ProducerVariables) {
    Model::Well producer = settings_.model()->wells().first();
    EXPECT_EQ(3, producer.variables.size());

    // BHP variable
    EXPECT_EQ(Model::WellVariableType::BHP, producer.variables[0].type);
    EXPECT_EQ(3, producer.variables[0].time_steps.size());
    EXPECT_STREQ("PROD-BHP-1", producer.variables[0].name.toLatin1().constData());

    // Transmissibility variable (All well blocks)
    EXPECT_EQ(Model::WellVariableType::Transmissibility, producer.variables[1].type);
    EXPECT_STREQ("PROD-TRANS-ALL", producer.variables[1].name.toLatin1().constData());
    EXPECT_EQ(2, producer.variables[1].blocks.size());
    EXPECT_EQ(2, producer.variables[1].blocks[0].position.i);
    EXPECT_EQ(3, producer.variables[1].blocks[1].position.i);

    // WellBlockPositionVariable
    EXPECT_EQ(Model::WellVariableType::WellBlockPosition, producer.variables[2].type);
    EXPECT_STREQ("PROD-WELLBLOCKS-ALL", producer.variables[2].name.toLatin1().constData());
    EXPECT_EQ(4, producer.variables[2].blocks.size());
    EXPECT_EQ(1, producer.variables[2].blocks[0].position.i);
    EXPECT_EQ(2, producer.variables[2].blocks[1].position.i);
    EXPECT_EQ(4, producer.variables[2].blocks[2].position.j);
    EXPECT_EQ(1, producer.variables[2].blocks[3].position.k);
}

TEST_F(ModelSettingsTest, InjectorWell) {
    Model::Well injector = settings_.model()->wells().last();
    EXPECT_STREQ("INJ", injector.name.toLatin1().constData());
    EXPECT_EQ(Model::WellType::Injector, injector.type);
    EXPECT_EQ(Model::ControlMode::RateControl, injector.controls.first().control_mode);
    EXPECT_FLOAT_EQ(1200.0, injector.controls.first().rate);
    EXPECT_EQ(0, injector.controls.first().time_step);
    EXPECT_EQ(Model::InjectionType::WaterInjection, injector.controls.first().injection_type);
    EXPECT_EQ(Model::WellState::WellOpen, injector.controls.first().state);
    EXPECT_EQ(Model::PreferedPhase::Water, injector.prefered_phase);
    EXPECT_FLOAT_EQ(0.75, injector.wellbore_radius);
    EXPECT_EQ(Model::Direction::X, injector.direction);

    EXPECT_EQ(1, injector.heel.i);
    EXPECT_EQ(1, injector.heel.j);
    EXPECT_EQ(1, injector.heel.k);

    EXPECT_EQ(Model::WellDefinitionType::WellSpline, injector.definition_type);
    EXPECT_EQ(2, injector.spline_points.size());
    EXPECT_FLOAT_EQ(12.0, injector.spline_points[1].y);
}

TEST_F(ModelSettingsTest, InjectorVariables) {
    Model::Well injector = settings_.model()->wells().last();
    EXPECT_EQ(1, injector.variables.size());
    EXPECT_EQ(Model::WellVariableType::SplinePoints, injector.variables.first().type);
    EXPECT_EQ(1, injector.variables.first().variable_spline_point_indices.last());
    EXPECT_EQ(1, injector.variables.first().time_steps.size());
    EXPECT_STREQ("INJ-SplinePoints-1", injector.variables.first().name.toLatin1().constData());
}

}
