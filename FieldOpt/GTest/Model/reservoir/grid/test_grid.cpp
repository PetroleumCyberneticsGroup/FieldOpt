#include <gtest/gtest.h>
#include "Model/reservoir/grid/grid.h"
#include "Model/reservoir/grid/eclgrid.h"
#include "Model/reservoir/grid/grid_exceptions.h"
#include "Model/reservoir//grid/cell.h"

using namespace Model::Reservoir::Grid;

namespace {

class GridTest : public ::testing::Test {
protected:
    GridTest() {
        grid_ = new ECLGrid(file_path_);
    }

    virtual ~GridTest() {
        delete grid_;
    }

    virtual void SetUp() {

    }

    virtual void TearDown() {

    }

    Grid* grid_;
    QString file_path_ = "../../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID";
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
    EXPECT_THROW(grid_->GetCell(20,10,10), CellIndexOutsideGridException);
}

TEST_F(GridTest, AttemptFindCellOutsideGrid) {
    EXPECT_THROW(grid_->GetCellEnvelopingPoint(100.0f, 1000.0, 7100.0), GridCellNotFoundException);
}

TEST_F(GridTest, GetCellEnvelopingPoint) {
    Cell cell = grid_->GetCellEnvelopingPoint(20, 300, 7050);
    EXPECT_EQ(cell.global_index(), 20);
}

TEST_F(GridTest, GetCellEnvelopingPointObject) {
    XYZCoordinate *xyz = new XYZCoordinate(20, 300, 7050);
    Cell cell = grid_->GetCellEnvelopingPoint(xyz);
    EXPECT_EQ(cell.global_index(), 20);
}

}
