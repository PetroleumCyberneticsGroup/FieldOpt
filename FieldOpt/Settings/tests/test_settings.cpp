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
  std::vector<int> verb_vector_ = std::vector<int>(11,0);
  EXPECT_NO_THROW(auto settings = ::Settings::Settings(
      TestResources::ExampleFilePaths::driver_example_,
      TestResources::ExampleFilePaths::directory_output_,
                      verb_vector_)); // <-verbosity level
}

TEST_F(SettingsTest, GlobalSettings) {
  std::vector<int> verb_vector_ = std::vector<int>(11,0);
  auto settings = ::Settings::Settings(TestResources::ExampleFilePaths::driver_example_,
                                       TestResources::ExampleFilePaths::directory_output_,
                                       verb_vector_); // <-verbosity level
  EXPECT_STREQ("TestRun", settings.name().toLatin1().constData());
  EXPECT_STREQ(TestResources::ExampleFilePaths::driver_example_.toLatin1().constData(),
               settings.driver_path().toLatin1().constData());
//    EXPECT_EQ(true, settings.verbosity()>0);
}

}
