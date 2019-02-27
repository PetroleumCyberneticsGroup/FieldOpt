/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "Settings/settings.h"
#include "test_resource_example_file_paths.hpp"

namespace {

class SettingsTest : public ::testing::Test {
protected:
    SettingsTest() {}
    virtual ~SettingsTest() {}

    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(SettingsTest, ConstructorAndTestFileValidity) {
    Paths paths;
    paths.SetPath(Paths::DRIVER_FILE, TestResources::ExampleFilePaths::driver_example_);
    paths.SetPath(Paths::OUTPUT_DIR, TestResources::ExampleFilePaths::directory_output_);
    paths.SetPath(Paths::SIM_DRIVER_FILE, TestResources::ExampleFilePaths::deck_horzwel_);
    EXPECT_NO_THROW(auto settings = ::Settings::Settings(paths));
}

TEST_F(SettingsTest, GlobalSettings) {
    Paths paths;
    paths.SetPath(Paths::DRIVER_FILE, TestResources::ExampleFilePaths::driver_example_);
    paths.SetPath(Paths::OUTPUT_DIR, TestResources::ExampleFilePaths::directory_output_);
    paths.SetPath(Paths::SIM_DRIVER_FILE, TestResources::ExampleFilePaths::deck_horzwel_);
    auto settings = ::Settings::Settings(paths);
    EXPECT_STREQ("TestRun", settings.name().toLatin1().constData());
}

}
