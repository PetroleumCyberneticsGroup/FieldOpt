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
    EXPECT_NO_THROW(auto settings = ::Settings::Settings(TestResources::ExampleFilePaths::driver_example_, TestResources::ExampleFilePaths::directory_output_));
}

TEST_F(SettingsTest, GlobalSettings) {
    auto settings = ::Settings::Settings(TestResources::ExampleFilePaths::driver_example_, TestResources::ExampleFilePaths::directory_output_);
    EXPECT_STREQ("TestRun", settings.name().toLatin1().constData());
    EXPECT_STREQ(TestResources::ExampleFilePaths::driver_example_.toLatin1().constData(), settings.driver_path().toLatin1().constData());
//    EXPECT_EQ(true, settings.verbosity()>0);
}

}
