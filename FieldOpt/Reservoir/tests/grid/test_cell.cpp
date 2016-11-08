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
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/tests/test_resource_grids.h"

using namespace Reservoir::Grid;

namespace {

    class CellTest : public ::testing::Test, TestResources::TestResourceGrids {
    protected:
        CellTest() {
            grid_ = grid_horzwel_;
        }

        virtual ~CellTest() { }
        virtual void SetUp() { }
        virtual void TearDown() { }

        Grid *grid_;
    };

    TEST_F(CellTest, Equality) {
        Cell cell_1 = grid_->GetCell(1); // Equal to cell_2_
        Cell cell_2 = grid_->GetCell(1, 0, 0); // Equal to cell_1_
        Cell cell_3 = grid_->GetCell(0);
        EXPECT_TRUE(cell_1.Equals(cell_2));
        EXPECT_FALSE(cell_1.Equals(cell_3));
    }

    TEST_F(CellTest, GlobalIndex) {
        Cell cell = grid_->GetCell(5, 0, 0);
        EXPECT_EQ(cell.global_index(), 5);
    }

    TEST_F(CellTest, IJKIndex) {
        Cell cell = grid_->GetCell(10);
        EXPECT_TRUE(cell.ijk_index().Equals(new IJKCoordinate(10, 0, 0)));
    }

    TEST_F(CellTest, Volume) {
        Cell cell = grid_->GetCell(0);
        EXPECT_EQ(cell.volume(), 1.5e+06);
    }

    TEST_F(CellTest, Center) {
        Cell cell = grid_->GetCell(0);
        Eigen::Vector3d center = Eigen::Vector3d(50.0, 150.0, 7025.0);
        EXPECT_TRUE(cell.center() == center);
    }

    TEST_F(CellTest, Corners) {
        auto corners = grid_->GetCell(0).corners();

        // Top layer
        Eigen::Vector3d top_sw = Eigen::Vector3d(0.0, 0.0, 7000.0);
        EXPECT_TRUE(corners[0] == top_sw);

        Eigen::Vector3d top_se = Eigen::Vector3d(100.0, 0.0, 7000.0);
        EXPECT_TRUE(corners[1] == top_se);

        Eigen::Vector3d top_nw = Eigen::Vector3d(0.0, 300.0, 7000.0);
        EXPECT_TRUE(corners[2] == top_nw);

        Eigen::Vector3d top_ne = Eigen::Vector3d(100.0, 300.0, 7000.0);
        EXPECT_TRUE(corners[3] == top_ne);

        // Bottom layer
        Eigen::Vector3d bottom_sw = Eigen::Vector3d(0.0, 0.0, 7050.0);
        EXPECT_TRUE(corners[4] == bottom_sw);

        Eigen::Vector3d bottom_se = Eigen::Vector3d(100.0, 0.0, 7050.0);
        EXPECT_TRUE(corners[5] == bottom_se);

        Eigen::Vector3d bottom_nw = Eigen::Vector3d(0.0, 300.0, 7050.0);
        EXPECT_TRUE(corners[6] == bottom_nw);

        Eigen::Vector3d bottom_ne = Eigen::Vector3d(100.0, 300.0, 7050.0);
        EXPECT_TRUE(corners[7] == bottom_ne);
    }

    TEST_F(CellTest, Properties) {
        auto cell = grid_->GetCell(1);
        EXPECT_FLOAT_EQ(0.25, cell.porosity());
        EXPECT_FLOAT_EQ(100, cell.permx());
        EXPECT_FLOAT_EQ(100, cell.permy());
        EXPECT_FLOAT_EQ(5, cell.permz());
    }

}

