#include <gtest/gtest.h>
#include <QList>
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include "tests/test_resource_grids.h"

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
        XYZCoordinate center = XYZCoordinate(50.0, 150.0, 7025.0);
        EXPECT_TRUE(cell.center().Equals(&center));
    }

    TEST_F(CellTest, Corners) {
        QList<Eigen::Vector3d> corners = grid_->GetCell(0).corners();

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

