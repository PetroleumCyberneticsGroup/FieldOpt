/******************************************************************************
 *
 * test_simulator.cpp
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

#include "Utilities/tests/test_resource_settings.hpp"

using namespace Utilities::Settings;

namespace {

class SimulatorSettingsTest : public ::testing::Test, public TestResources::TestResourceSettings {
protected:
    SimulatorSettingsTest()
    { }
    virtual ~SimulatorSettingsTest() {}

    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(SimulatorSettingsTest, Fields) {
    EXPECT_EQ(settings_simulator_->type(), Simulator::SimulatorType::ECLIPSE);
    EXPECT_EQ(settings_simulator_->commands()->size(), 1);
    EXPECT_STREQ("../examples/ECLIPSE/HORZWELL/HORZWELL.DATA", settings_simulator_->driver_file_path().toLatin1().constData());
}

}
