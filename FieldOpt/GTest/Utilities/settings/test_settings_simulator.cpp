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

#include "Utilities/settings/settings.h"
#include "Utilities/settings/simulator.h"

using namespace Utilities::Settings;

namespace {

class SimulatorSettingsTest : public ::testing::Test {
protected:
    SimulatorSettingsTest()
        : settings_(driver_file_path_, output_directory_)
    {
    }
    virtual ~SimulatorSettingsTest() {}

    virtual void SetUp() {}
    virtual void TearDown() {}

    QString driver_file_path_ = "../../FieldOpt/GTest/Utilities/driver/driver.json";
    QString output_directory_ = "/home/einar/Documents/GitHub/PCG/fieldopt_output";
    Settings settings_;
};

TEST_F(SimulatorSettingsTest, Fields) {
    EXPECT_EQ(settings_.simulator()->type(), Simulator::SimulatorType::ECLIPSE);
    EXPECT_EQ(settings_.simulator()->commands()->size(), 1);
    EXPECT_STREQ("../../examples/ECLIPSE/HORZWELL/HORZWELL.DATA", settings_.simulator()->driver_file_path().toLatin1().constData());
}

}
