/******************************************************************************
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>,
   Brage Strand Kristoffersen <brage_sk@hotmail.com>

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
#include "Model/cpp-spline/src/Bezier.h"
#include "Reservoir/tests/test_resource_grids.h"
#include "Model/tests/test_resource_model.h"
#include "Settings/tests/test_resource_example_file_paths.hpp"
#include "Model/wells/wellbore/k_locked_polar_spline.h"
#include <math.h>

namespace {


    class KLockedPolarSplineTest : public ::testing::Test,
                            public TestResources::TestResourceModel,
                            public TestResources::TestResourceGrids {
    protected:
        KLockedPolarSplineTest() {
        }

    };

    TEST_F(KLockedPolarSplineTest, Constructor) {
        Paths paths;
        paths.SetPath(Paths::GRID_FILE, TestResources::ExampleFilePaths::grid_5spot_);
        auto settings = Settings::Model(TestResources::TestResourceModelSettingSnippets::model_k_locked_polar_well(), paths);
        auto wsettings = settings.wells()[0];

        // Reservoir size/dimensions:
        // 1440 x 1440 m, 1700-1724 m
        // 60x60x1

        wsettings.polar_spline.azimuth = 0;
        wsettings.polar_spline.length = 1400;
        wsettings.spline_heel.x = 12;
        wsettings.spline_heel.y = 12;
        wsettings.spline_heel.z = 1712;

        auto varcont = new Model::Properties::VariablePropertyContainer();
        auto well = Model::Wells::Wellbore::KLockedPolarSpline(wsettings, varcont, TestResources::TestResourceGrids::grid_5spot_, nullptr);
        auto well_blocks = well.GetWellBlocks();

    }

//    TEST_F(KLockedPolarSplineTest, Endblocks) {
//        Paths paths;
//        paths.SetPath(Paths::GRID_FILE, TestResources::ExampleFilePaths::grid_5spot_);
//        auto settings = Settings::Model(TestResources::TestResourceModelSettingSnippets::model_polar_well(), paths);
//        auto wsettings = settings.wells()[0];
//
//        // Reservoir size/dimensions:
//        // 1440 x 1440 m, 1700-1724 m
//        // 60x60x1
//
//        wsettings.polar_spline.elevation = 90;
//        wsettings.polar_spline.azimuth = 180;
//        wsettings.polar_spline.length = 1440;
//        wsettings.polar_spline.midpoint.x = 720;
//        wsettings.polar_spline.midpoint.y = 732;
//        wsettings.polar_spline.midpoint.z = 1712;
//
//        auto varcont = new Model::Properties::VariablePropertyContainer();
//        auto well = Model::Wells::Wellbore::KLockedPolarSpline(wsettings, varcont, TestResources::TestResourceGrids::grid_5spot_, nullptr);
//        auto well_blocks = well.GetWellBlocks();
//
//        auto first_block = well_blocks->front();
//        auto last_block = well_blocks->back();
//
//        EXPECT_EQ(60, well_blocks->size());
//
//        EXPECT_EQ(1, first_block->k());
//        EXPECT_EQ(1, last_block->k());
//
//        EXPECT_EQ(1, first_block->i());
//        EXPECT_EQ(60, last_block->i());
//
//        EXPECT_EQ(31, first_block->j());
//        EXPECT_EQ(31, last_block->j());
//
//    }
//
//    TEST_F(KLockedPolarSplineTest, Tilting) {
//        Paths paths;
//        paths.SetPath(Paths::GRID_FILE, TestResources::ExampleFilePaths::grid_5spot_);
//        auto settings = Settings::Model(TestResources::TestResourceModelSettingSnippets::model_polar_well(), paths);
//        auto wsettings = settings.wells()[0];
//
//        // Reservoir size/dimensions:
//        // 1440 x 1440 m, 1700-1724 m
//        // 60x60x1
//
//        wsettings.polar_spline.elevation = 90+5;
//        wsettings.polar_spline.azimuth = 180;
//        wsettings.polar_spline.length = 1440;
//        wsettings.polar_spline.midpoint.x = 720;
//        wsettings.polar_spline.midpoint.y = 732;
//        wsettings.polar_spline.midpoint.z = 1712;
//
//        auto varcont = new Model::Properties::VariablePropertyContainer();
//        auto well = Model::Wells::Wellbore::KLockedPolarSpline(wsettings, varcont, TestResources::TestResourceGrids::grid_5spot_, nullptr);
//        auto well_blocks = well.GetWellBlocks();
//
//        auto first_block = well_blocks->front();
//        auto last_block = well_blocks->back();
//
//        EXPECT_EQ(12, well_blocks->size());
//
//
//    }
//    TEST_F(KLockedPolarSplineTest, Screwing) {
//        Paths paths;
//        paths.SetPath(Paths::GRID_FILE, TestResources::ExampleFilePaths::grid_5spot_);
//        auto settings = Settings::Model(TestResources::TestResourceModelSettingSnippets::model_polar_well(), paths);
//        auto wsettings = settings.wells()[0];
//
//        // Reservoir size/dimensions:
//        // 1440 x 1440 m, 1700-1724 m
//        // 60x60x1
//
//        wsettings.polar_spline.elevation = 90;
//        wsettings.polar_spline.azimuth = 180+45;
//        wsettings.polar_spline.length = sqrt(pow(1440,2)+pow(1440,2));
//        wsettings.polar_spline.midpoint.x = 720;
//        wsettings.polar_spline.midpoint.y = 732;
//        wsettings.polar_spline.midpoint.z = 1712;
//
//        auto varcont = new Model::Properties::VariablePropertyContainer();
//        auto well = Model::Wells::Wellbore::KLockedPolarSpline(wsettings, varcont, TestResources::TestResourceGrids::grid_5spot_, nullptr);
//        auto well_blocks = well.GetWellBlocks();
//
//        auto first_block = well_blocks->front();
//        auto last_block = well_blocks->back();
//
//        EXPECT_EQ(119, well_blocks->size());
//
//        EXPECT_EQ(1, first_block->k());
//        EXPECT_EQ(1, last_block->k());
//
//        EXPECT_EQ(1, first_block->i());
//        EXPECT_EQ(1, first_block->j());
//
//        EXPECT_EQ(60, last_block->j());
//        EXPECT_EQ(60, last_block->i());
//
//
//    }
}
