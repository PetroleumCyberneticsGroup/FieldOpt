#include <gtest/gtest.h>
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "tests/test_resource_grids.h"

using namespace Reservoir::Grid;

namespace {

    class GridTest : public ::testing::Test, TestResources::TestResourceGrids {
    protected:
        GridTest() : Test(), TestResourceGrids() {
            grid_ = grid_horzwel_;
        }

        virtual ~GridTest() {
            delete grid_;
        }

        virtual void SetUp() { }
        virtual void TearDown() { }

        Grid* grid_;
    };

    TEST_F(GridTest, GridRead) {
        // Dimensions() should throw an error if the reservoir has not been propely set.
        EXPECT_NO_THROW(grid_->Dimensions());
    }

    TEST_F(GridTest, CheckDimensions) {
        Grid::Dims dims = grid_->Dimensions();
        EXPECT_EQ(dims.nx, 20);
        EXPECT_EQ(dims.ny, 9);
        EXPECT_EQ(dims.nz, 9);
    }

    TEST_F(GridTest, GetCellByGlobalIndex) {
        // Cell 0 should have IJK coordinates (0,0,0) and volume 1.5e+06
        Cell cell = grid_->GetCell(0);
        EXPECT_EQ(cell.ijk_index().i(), 0);
        EXPECT_EQ(cell.ijk_index().j(), 0);
        EXPECT_EQ(cell.ijk_index().k(), 0);
        EXPECT_EQ(cell.volume(), 1.5e+06);
    }

    TEST_F(GridTest, GetCellByIJKIndex) {
        // Cell (1,0,0) should have global index 1
        Cell cell = grid_->GetCell(1, 0, 0);
        EXPECT_EQ(cell.global_index(), 1);
    }

    TEST_F(GridTest, GetCellByIJKObject) {
        // Cell (2,0,0) should have global index 2
        IJKCoordinate *ijk = new IJKCoordinate(2, 0, 0);
        Cell cell = grid_->GetCell(ijk);
        EXPECT_EQ(cell.global_index(), 2);
    }

    TEST_F(GridTest, AttemptGetCellWithIndexOutsideGrid) {
        EXPECT_THROW(grid_->GetCell(20,10,10), std::runtime_error);
    }

    TEST_F(GridTest, AttemptFindCellOutsideGrid) {
        EXPECT_THROW(grid_->GetCellEnvelopingPoint(100.0f, 1000.0, 7100.0), std::runtime_error);
    }

    TEST_F(GridTest, GetCellEnvelopingPoint) {
        Cell cell = grid_->GetCellEnvelopingPoint(21, 301, 7025);
        EXPECT_EQ(cell.global_index(), 20);
    }

    TEST_F(GridTest, GetCellEnvelopingPointObject) {
        Eigen::Vector3d xyz = Eigen::Vector3d(21, 301, 7025);
        Cell cell = grid_->GetCellEnvelopingPoint(xyz);
        EXPECT_EQ(cell.global_index(), 20);
    }

    TEST_F(GridTest, GetCellEnvelopingPointAdditional) {
        auto cell_100 = grid_->GetCell(0);
        auto cell_200 = grid_->GetCell(1);
        auto cell_010 = grid_->GetCell(0, 1, 0);
        auto cell_001 = grid_->GetCell(0, 0, 1);

        EXPECT_TRUE(cell_100.EnvelopsPoint(Eigen::Vector3d(1,1,7001))); // Should be inside first cell
        EXPECT_TRUE(cell_100.EnvelopsPoint(Eigen::Vector3d(100,1,7001))); // Should be on the bound of first and second cell

        EXPECT_TRUE(cell_200.EnvelopsPoint(Eigen::Vector3d(100,1,7001))); // Should be on the bound of first and second cell
        EXPECT_FALSE(cell_200.EnvelopsPoint(Eigen::Vector3d(99,1,7001))); // Should be inside first cell

        EXPECT_TRUE(cell_010.EnvelopsPoint(Eigen::Vector3d(1, 310, 7001))); // Should be inside first cell
        EXPECT_FALSE(cell_010.EnvelopsPoint(Eigen::Vector3d(1,1,7001))); // Should be inside the first cell
        EXPECT_TRUE(cell_010.EnvelopsPoint(Eigen::Vector3d(1,300,7001))); // Should be on the interface between the first cell and cell 010
        EXPECT_TRUE(cell_100.EnvelopsPoint(Eigen::Vector3d(1,300,7001))); // Should be on the interface between the first cell and cell 010

        EXPECT_TRUE(cell_001.EnvelopsPoint(Eigen::Vector3d(50,50,7055))); // Should be inside cell 001
        EXPECT_FALSE(cell_001.EnvelopsPoint(Eigen::Vector3d(1,1,7049))); // Should be inside cell 100
        EXPECT_TRUE(cell_001.EnvelopsPoint(Eigen::Vector3d(1,1,7050))); // Should be on the interface between cell 001 and 100
        EXPECT_TRUE(cell_100.EnvelopsPoint(Eigen::Vector3d(1,1,7050))); // Should be on the interface between cell 001 and 100
    }

}
