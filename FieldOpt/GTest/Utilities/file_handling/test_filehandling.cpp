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

#include "Utilities/file_handling/filehandling.h"

namespace {

class FileHandlingTest : public ::testing::Test {
protected:
    FileHandlingTest() {}
    virtual ~FileHandlingTest() {}

    virtual void SetUp() {}
    virtual void TearDown() {}

    QString driver_file_path_ = "../../FieldOpt/GTest/Utilities/driver/driver.json";
    QString driver_directory_path_ = "../../FieldOpt/GTest/Utilities/driver";


};

TEST_F(FileHandlingTest, Existance) {
    EXPECT_TRUE(::Utilities::FileHandling::FileExists(driver_file_path_));
    EXPECT_FALSE(::Utilities::FileHandling::FileExists(driver_file_path_ + "wrong"));
    EXPECT_FALSE(::Utilities::FileHandling::FileExists(driver_directory_path_));

    EXPECT_TRUE(::Utilities::FileHandling::DirectoryExists(driver_directory_path_));
    EXPECT_FALSE(::Utilities::FileHandling::DirectoryExists(driver_directory_path_ + "wrong"));
    EXPECT_FALSE(::Utilities::FileHandling::DirectoryExists(driver_file_path_));
    EXPECT_FALSE(::Utilities::FileHandling::DirectoryIsEmpty(driver_directory_path_));
}

TEST_F(FileHandlingTest, FileReading) {
    EXPECT_LE(160, ::Utilities::FileHandling::ReadFileToStringList(driver_file_path_)->size());
}


}
