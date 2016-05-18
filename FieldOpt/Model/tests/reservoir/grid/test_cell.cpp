#include <gtest/gtest.h>
#include <QList>
#include "Model/reservoir/grid/grid.h"
#include "Model/reservoir/grid/eclgrid.h"
#include "Model/reservoir/grid/grid_exceptions.h"
#include "Model/reservoir/grid/cell.h"

using namespace Model::Reservoir::Grid;

namespace {

class CellTest : public ::testing::Test {
protected:
    CellTest() {
        grid_ = new ECLGrid(file_path_);
    }

    virtual ~CellTest() {
        delete grid_;
    }

    virtual void SetUp() { }

    virtual void TearDown() { }

    Grid *grid_;
    QString file_path_ = "../../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID";
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
    QList<XYZCoordinate> corners = grid_->GetCell(0).corners();

    // Top layer
    XYZCoordinate top_sw = XYZCoordinate(0.0, 0.0, 7000.0);
    EXPECT_TRUE(corners.at(0).Equals(top_sw));

    XYZCoordinate top_se = XYZCoordinate(100.0, 0.0, 7000.0);
    EXPECT_TRUE(corners.at(1).Equals(top_se));

    XYZCoordinate top_nw = XYZCoordinate(0.0, 300.0, 7000.0);
    EXPECT_TRUE(corners.at(2).Equals(top_nw));

    XYZCoordinate top_ne = XYZCoordinate(100.0, 300.0, 7000.0);
    EXPECT_TRUE(corners.at(3).Equals(top_ne));

    // Bottom layer
    XYZCoordinate bottom_sw = XYZCoordinate(0.0, 0.0, 7050.0);
    EXPECT_TRUE(corners.at(4).Equals(bottom_sw));

    XYZCoordinate bottom_se = XYZCoordinate(100.0, 0.0, 7050.0);
    EXPECT_TRUE(corners.at(5).Equals(bottom_se));

    XYZCoordinate bottom_nw = XYZCoordinate(0.0, 300.0, 7050.0);
    EXPECT_TRUE(corners.at(6).Equals(bottom_nw));

    XYZCoordinate bottom_ne = XYZCoordinate(100.0, 300.0, 7050.0);
    EXPECT_TRUE(corners.at(7).Equals(bottom_ne));
}

TEST_F(CellTest, Properties) {
    auto cell = grid_->GetCell(1);
    EXPECT_FLOAT_EQ(0.25, cell.porosity());
    EXPECT_FLOAT_EQ(100, cell.permx());
    EXPECT_FLOAT_EQ(100, cell.permy());
    EXPECT_FLOAT_EQ(5, cell.permz());
}

}

