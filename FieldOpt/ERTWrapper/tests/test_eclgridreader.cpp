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
#include <boost/lexical_cast.hpp>
#include "ERTWrapper/eclgridreader.h"

using namespace ERTWrapper::ECLGrid;

namespace {

class ECLGridReaderTest : public ::testing::Test {
 protected:
  ECLGridReaderTest() {
      ecl_grid_reader_horzwell_ = new ECLGridReader();
      ecl_grid_reader_brugge_ = new ECLGridReader();
      ecl_grid_reader_5spot_ = new ECLGridReader();
      ecl_grid_reader_5spot_exp_ = new ECLGridReader();
  }

  virtual ~ECLGridReaderTest() {
      delete ecl_grid_reader_horzwell_;
      delete ecl_grid_reader_brugge_;
      delete ecl_grid_reader_5spot_;
      delete ecl_grid_reader_5spot_exp_;
  }

  virtual void SetUp() {
      ecl_grid_reader_horzwell_->ReadEclGrid(grid_horzwell_);
      ecl_grid_reader_brugge_->ReadEclGrid(grid_brugge_);
      ecl_grid_reader_5spot_->ReadEclGrid(grid_5spot_);
      ecl_grid_reader_5spot_exp_->ReadEclGrid(grid_5spot_exp_);
  }

  virtual void TearDown() { }

  ECLGridReader* ecl_grid_reader_horzwell_;
  ECLGridReader* ecl_grid_reader_brugge_;
  ECLGridReader* ecl_grid_reader_5spot_;
  ECLGridReader* ecl_grid_reader_5spot_exp_;

  // Simulation grid files
  std::string grid_horzwell_ = "../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID";
  std::string grid_brugge_ = "../examples/ECLIPSE/Brugge_xyz/BRUGGE.EGRID";
  std::string grid_5spot_ = "../examples/ECLIPSE/5spot/ECL_5SPOT.EGRID";
  std::string grid_5spot_exp_ = "../examples/ECLIPSE/5spot_exp/ECL_5SPOT_NOROT.EGRID";

  // Objects declared here can be used by all tests in this test case.
  void PrintCell(ECLGridReader::Cell cell) {
      std::cout << "Cell " << cell.global_index << std::endl;
      std::cout << "\tCenter: ("
                << cell.center.x() << ", "
                << cell.center.y() << ", "
                << cell.center.z() << ")"
                << std::endl;
      std::cout << "\tCorners: " << std::endl;
      for (int i = 0; i < 8; ++i) {
          std::cout << "\t\t("
                    << cell.corners.at(i).x() << ", "
                    << cell.corners.at(i).y() << ", "
                    << cell.corners.at(i).z() << ")"
                    << std::endl;
      }
  }
};

TEST_F(ECLGridReaderTest, ReadGrid) {
    EXPECT_EQ(1620, ecl_grid_reader_horzwell_->ActiveCells());
}

TEST_F(ECLGridReaderTest, ConvertIJKToGlobalIndex) {
    ECLGridReader::IJKIndex index_1;
    index_1.i = 0; index_1.j = 0; index_1.k = 0;
    int global_index1 = ecl_grid_reader_horzwell_->ConvertIJKToGlobalIndex(index_1);
    EXPECT_EQ(global_index1, 0);

    ECLGridReader::IJKIndex index_2;
    index_2.i = 1; index_2.j = 0; index_2.k = 0;
    int global_index2 = ecl_grid_reader_horzwell_->ConvertIJKToGlobalIndex(index_2);
    EXPECT_EQ(global_index2, 1);
}

TEST_F(ECLGridReaderTest, ConvertGlobalIndexToIJK) {
    ECLGridReader::IJKIndex ijk1 = ecl_grid_reader_horzwell_->ConvertGlobalIndexToIJK(0);
    ECLGridReader::IJKIndex ijk2 = ecl_grid_reader_horzwell_->ConvertGlobalIndexToIJK(1);
    EXPECT_EQ(ijk1.i, 0); EXPECT_EQ(ijk1.j, 0); EXPECT_EQ(ijk1.k, 0);
    EXPECT_EQ(ijk2.i, 1); EXPECT_EQ(ijk2.j, 0); EXPECT_EQ(ijk2.k, 0);
}

TEST_F(ECLGridReaderTest, CheckDimensions) {
    ECLGridReader::Dims dims = ecl_grid_reader_horzwell_->Dimensions();
    EXPECT_EQ(20, dims.nx);
    EXPECT_EQ(9, dims.ny);
    EXPECT_EQ(9, dims.nz);
}

TEST_F(ECLGridReaderTest, GetCell) {
    ECLGridReader::Cell cell = ecl_grid_reader_horzwell_->GetGridCell(1);
    ECLGridReader::Cell cell2 = ecl_grid_reader_horzwell_->GetGridCell(0);
    //PrintCell(&cell);
    EXPECT_EQ(1, cell.global_index);
    EXPECT_EQ(8, cell.corners.size());
    EXPECT_EQ(cell.corners.at(0).x(), 100);
    EXPECT_EQ(cell.corners.at(7).z(), 7050);
    EXPECT_EQ(cell.center.x(), 150);
    EXPECT_EQ(cell.volume, 1.5e+06);
    EXPECT_EQ(0, cell2.global_index);
}

TEST_F(ECLGridReaderTest, CellProperties) {
    auto cell_1 = ecl_grid_reader_horzwell_->GetGridCell(1);
    auto cell_1000 = ecl_grid_reader_horzwell_->GetGridCell(1000);
    EXPECT_FLOAT_EQ(0.25, cell_1.porosity);
    EXPECT_FLOAT_EQ(100, cell_1.permx);
    EXPECT_FLOAT_EQ(100, cell_1.permy);
    EXPECT_FLOAT_EQ(5, cell_1.permz);
    EXPECT_FLOAT_EQ(0.25, cell_1000.porosity);
    EXPECT_FLOAT_EQ(100, cell_1000.permx);
    EXPECT_FLOAT_EQ(100, cell_1000.permy);
    EXPECT_FLOAT_EQ(5, cell_1000.permz);
}

TEST_F(ECLGridReaderTest, GlobalIndexOfCellEncompasingPoint) {
    int global_index_1 = ecl_grid_reader_horzwell_->GlobalIndexOfCellEnvelopingPoint(125, 50, 7010, 1); // Should be 1
    int global_index_2 = ecl_grid_reader_horzwell_->GlobalIndexOfCellEnvelopingPoint(700, 800, 7100, 0); // Should be inside grid
    int global_index_3 = ecl_grid_reader_horzwell_->GlobalIndexOfCellEnvelopingPoint(0, 0, 0, 0); // Should be outside grid
    EXPECT_EQ(global_index_1, 1);
    EXPECT_GT(global_index_2, -1);
    EXPECT_EQ(global_index_3, -1);
}

// Grid tests
TEST_F(ECLGridReaderTest, GetGridIndices) {
    ecl_grid_reader_brugge_->GetGridIndices();
    auto gidx = ecl_grid_reader_brugge_->gidx_;
    EXPECT_EQ(60048, gidx.n_total);
    EXPECT_EQ(43847, gidx.n_active);
    EXPECT_EQ(60048, gidx.idx_actnum.rows());
    EXPECT_EQ(43847, gidx.idx_active.rows());

    cout << "gidx.dat_actnum.rows():" << gidx.dat_actnum.rows() << endl;
    cout << "gidx.idx_actnum.rows():" << gidx.idx_actnum.rows() << endl;
    cout << "gidx.idx_active.rows():" << gidx.idx_active.rows() << endl;

    for (int ii=0; ii < 20*6; ii++) {
        cout << gidx.idx_active(ii) << " ";
        if (remainder(ii, 20) == 0 && ii > 0) {
            cout << endl;
        }
    }
    cout << endl;
}

TEST_F(ECLGridReaderTest, GetGridCoord_5spot) {

    // Test 5spot grid --------------------------------------------
    ecl_grid_reader_5spot_exp_->GetCOORDZCORNData();
    auto gidx_5spot_exp_ = ecl_grid_reader_5spot_exp_->gidx_;
    int n_total = gidx_5spot_exp_.n_total;
    int n_active = gidx_5spot_exp_.n_active;
    int n_inactive = n_total - n_active;

    EXPECT_EQ(3600, n_total); // n_total*8 = 3600 = 28800
    EXPECT_EQ(3551, n_active);
    EXPECT_EQ(49, n_inactive); // 7 x 7 hole

    auto gridData_5spot_exp_ = ecl_grid_reader_5spot_exp_->gridData_;
    auto coord_5spot_exp_ = gridData_5spot_exp_.coord;

    // 5spot grid: 60 x 60 x 1
    int coord_sz;
    // 28800, 22326
    coord_sz = n_total*8 - n_inactive; // ????
    EXPECT_EQ(22326, coord_5spot_exp_.size());
    cout << "coord_kw_sz [5spot_exp_]: " << coord_5spot_exp_.size() << endl;

}

//TEST_F(ECLGridReaderTest, GetGridCoord_Brugge) {
//
//    // Test Brugge grid ------------------------------------------
//    ecl_grid_reader_brugge_->GetCOORDZCORNData();
//    auto gidx_brugge_ = ecl_grid_reader_brugge_->gidx_;
//    auto gridData_brugge_ = ecl_grid_reader_brugge_->gridData_;
//    auto coord_brugge_ = gridData_brugge_.coord;
////    ecl_grid_reader_5spot_exp_->RotateGrid();
//
//    // Brugge grid: .. x .. x 9
//    EXPECT_EQ(60048, coord_brugge_.size());
//    cout << "coord_kw_sz [brugge_]: " << coord_brugge_.size() << endl;
//}



//TEST_F(ECLGridReaderTest, BoundingCentroids) {
//    ECLGridReader *norne_reader = new ECLGridReader();
//    norne_reader->ReadEclGrid(grid_brugge_);
//    auto bounding_centroids = norne_reader->GetBoundaryCentroids();
//    cout << endl;
//    for (auto centroid : bounding_centroids) {
//        cout << boost::lexical_cast<string>(centroid.x())
//             << ", " << boost::lexical_cast<string>(centroid.y())
//             << ", " << boost::lexical_cast<string>(centroid.z()) << endl;
//    }
//}

}
