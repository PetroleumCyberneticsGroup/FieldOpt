/******************************************************************************
 *
 *
 *
 * Created: 28.10.2015 2015 by einar
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

#include "filehandling.hpp"
#include "Settings/tests/test_resource_example_file_paths.hpp"

namespace {

class FileHandlingTest : public ::testing::Test {
protected:
    FileHandlingTest() {}
    virtual ~FileHandlingTest() {}

    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(FileHandlingTest, Existance) {
    EXPECT_TRUE(::Utilities::FileHandling::FileExists(TestResources::ExampleFilePaths::driver_example_));
    EXPECT_FALSE(::Utilities::FileHandling::FileExists(TestResources::ExampleFilePaths::driver_example_ + "wrong"));
    EXPECT_FALSE(::Utilities::FileHandling::FileExists(TestResources::ExampleFilePaths::directory_output_));

    EXPECT_TRUE(::Utilities::FileHandling::DirectoryExists(TestResources::ExampleFilePaths::directory_output_));
    EXPECT_FALSE(::Utilities::FileHandling::DirectoryExists(TestResources::ExampleFilePaths::directory_output_ + "wrong"));
    EXPECT_FALSE(::Utilities::FileHandling::DirectoryExists(TestResources::ExampleFilePaths::driver_example_));
}

TEST_F(FileHandlingTest, FileReading) {
    EXPECT_LE(155, ::Utilities::FileHandling::ReadFileToStringList(TestResources::ExampleFilePaths::driver_example_)->size());
}


}
