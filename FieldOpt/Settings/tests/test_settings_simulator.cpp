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
