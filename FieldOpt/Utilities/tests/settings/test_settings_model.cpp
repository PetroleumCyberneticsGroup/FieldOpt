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

#include "Utilities/tests/test_resource_settings.h"

using namespace Utilities::Settings;

namespace {

    class ModelSettingsTest : public ::testing::Test, public TestResources::TestResourceSettings {
    protected:
        ModelSettingsTest()
        { }
        virtual ~ModelSettingsTest() {}

        virtual void SetUp() {}
        virtual void TearDown() {}
    };

    TEST_F(ModelSettingsTest, Reservoir) {
        EXPECT_EQ(settings_model_->reservoir().type, Model::ReservoirGridSourceType::ECLIPSE);
        EXPECT_STREQ("../examples/ADGPRS/5spot/ECL_5SPOT.EGRID", settings_model_->reservoir().path.toLatin1().constData());
    }

    TEST_F(ModelSettingsTest, ControlTimes) {
        EXPECT_EQ(4, settings_model_->control_times().size());
        EXPECT_EQ(365, settings_model_->control_times().last());
    }

    TEST_F(ModelSettingsTest, ProducerWell) {
        Model::Well producer = settings_model_->wells().first();
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

        EXPECT_EQ(4, producer.well_blocks[3].i);
        EXPECT_EQ(4, producer.well_blocks.size());
    }

    TEST_F(ModelSettingsTest, ProducerCompletions) {
        Model::Well producer = settings_model_->wells().first();
        EXPECT_EQ(Model::WellCompletionType::Perforation, producer.well_blocks.first().completion.type);
    }

    TEST_F(ModelSettingsTest, ProducerVariables) {
        Model::Well producer = settings_model_->wells().first();

        // BHP variable
        EXPECT_EQ(Model::WellVariableType::BHP, producer.variables[0].type);
        EXPECT_EQ(3, producer.variables[0].time_steps.size());
        EXPECT_STREQ("PROD-BHP-1", producer.variables[0].name.toLatin1().constData());
    }

    TEST_F(ModelSettingsTest, InjectorWell) {
        Model::Well injector = settings_model_->wells().last();
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

        EXPECT_EQ(Model::WellDefinitionType::WellSpline, injector.definition_type);
        EXPECT_FLOAT_EQ(12.0, injector.spline_toe.y);
    }

}
