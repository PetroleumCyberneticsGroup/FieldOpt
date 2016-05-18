/******************************************************************************
 *
 *
 *
 * Created: 05.11.2015 2015 by einar
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
#include "Model/reservoir/reservoir.h"
#include "Utilities/settings/model.h"

using namespace Model::Reservoir;

namespace {

class ReservoirTest : public ::testing::Test {
protected:
    ReservoirTest() {
        settings_ = new ::Utilities::Settings::Settings(driver_file_path_, output_directory_);
        reservoir_ = new Reservoir(settings_->model()->reservoir());
    }

    virtual ~ReservoirTest() {}

    virtual void SetUp() {}

    virtual void TearDown() {}

    ::Utilities::Settings::Settings *settings_;
    Reservoir *reservoir_;
    QString driver_file_path_ = "../../FieldOpt/GTest/Utilities/driver/driver.json";
    QString output_directory_ = "/home/einar/Documents/GitHub/PCG/fieldopt_output";
};

TEST_F(ReservoirTest, Constructor) {
    EXPECT_TRUE(true);
}

TEST_F(ReservoirTest, CheckDimensions) {
    Grid::Grid::Dims dims = reservoir_->grid()->Dimensions();
    EXPECT_EQ(dims.nx, 60);
    EXPECT_EQ(dims.ny, 60);
    EXPECT_EQ(dims.nz, 1);
}

}
