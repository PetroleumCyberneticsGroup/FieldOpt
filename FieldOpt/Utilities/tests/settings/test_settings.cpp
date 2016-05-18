/******************************************************************************
 *
 * test_settings.cpp
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

using namespace Utilities::Settings;

namespace {

class SettingsTest : public ::testing::Test {
protected:
    SettingsTest() {}
    virtual ~SettingsTest() {}

    virtual void SetUp() {}
    virtual void TearDown() {}

    QString driver_file_path_ = "../../FieldOpt/GTest/Utilities/driver/driver.json";
    QString output_directory_ = "/home/einar/Documents/GitHub/PCG/fieldopt_output";
};

TEST_F(SettingsTest, ConstructorAndTestFileValidity) {
    EXPECT_NO_THROW(Settings settings = Settings(driver_file_path_, output_directory_));
}

TEST_F(SettingsTest, GlobalSettings) {
    Settings settings = Settings(driver_file_path_, output_directory_);
    EXPECT_STREQ("TestRun", settings.name().toLatin1().constData());
    EXPECT_STREQ(driver_file_path_.toLatin1().constData(), settings.driver_path().toLatin1().constData());
    EXPECT_EQ(false, settings.verbose());
}

}
