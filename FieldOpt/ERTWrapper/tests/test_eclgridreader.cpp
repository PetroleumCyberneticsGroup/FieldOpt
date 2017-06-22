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
      ecl_grid_reader_ = new ECLGridReader();
  }

  virtual ~ECLGridReaderTest() {
      delete ecl_grid_reader_;
  }

  virtual void SetUp() {
      ecl_grid_reader_->ReadEclGrid(file_name_);
  }

  virtual void TearDown() { }

  ECLGridReader* ecl_grid_reader_;
  std::string file_name_ = "../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID";
  std::string norne_file_name_ = "/home/einar/testpit/Brugge_xyz_snopt/brugge_xyz_snopt/ECL/BRUGGE.EGRID";

  // Objects declared here can be used by all tests in this test case.

  void PrintCell(ECLGridReader::Cell cell) {
      std::cout << "Cell " << cell.global_index << std::endl;
      std::cout << "\tCenter: (" << cell.center.x() << ", " << cell.center.y() << ", " << cell.center.z() << ")" << std::endl;
      std::cout << "\tCorners: " << std::endl;
      for (int i = 0; i < 8; ++i) {
          std::cout << "\t\t(" << cell.corners.at(i).x() << ", " << cell.corners.at(i).y() << ", " << cell.corners.at(i).z() << ")" << std::endl;
      }
  }
};

TEST_F(ECLGridReaderTest, ReadGrid) {
    EXPECT_EQ(1620, ecl_grid_reader_->ActiveCells());
}

TEST_F(ECLGridReaderTest, ConvertIJKToGlobalIndex) {
    ECLGridReader::IJKIndex index_1;
    index_1.i = 0; index_1.j = 0; index_1.k = 0;
    int global_index1 = ecl_grid_reader_->ConvertIJKToGlobalIndex(index_1);
    EXPECT_EQ(global_index1, 0);

    ECLGridReader::IJKIndex index_2;
    index_2.i = 1; index_2.j = 0; index_2.k = 0;
    int global_index2 = ecl_grid_reader_->ConvertIJKToGlobalIndex(index_2);
    EXPECT_EQ(global_index2, 1);
}

TEST_F(ECLGridReaderTest, ConvertGlobalIndexToIJK) {
    ECLGridReader::IJKIndex ijk1 = ecl_grid_reader_->ConvertGlobalIndexToIJK(0);
    ECLGridReader::IJKIndex ijk2 = ecl_grid_reader_->ConvertGlobalIndexToIJK(1);
    EXPECT_EQ(ijk1.i, 0); EXPECT_EQ(ijk1.j, 0); EXPECT_EQ(ijk1.k, 0);
    EXPECT_EQ(ijk2.i, 1); EXPECT_EQ(ijk2.j, 0); EXPECT_EQ(ijk2.k, 0);
}

TEST_F(ECLGridReaderTest, CheckDimensions) {
    ECLGridReader::Dims dims = ecl_grid_reader_->Dimensions();
    EXPECT_EQ(20, dims.nx);
    EXPECT_EQ(9, dims.ny);
    EXPECT_EQ(9, dims.nz);
}

TEST_F(ECLGridReaderTest, GetCell) {
    ECLGridReader::Cell cell = ecl_grid_reader_->GetGridCell(1);
    ECLGridReader::Cell cell2 = ecl_grid_reader_->GetGridCell(0);
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
    auto cell_1 = ecl_grid_reader_->GetGridCell(1);
    auto cell_1000 = ecl_grid_reader_->GetGridCell(1000);
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
    int global_index_1 = ecl_grid_reader_->GlobalIndexOfCellEnvelopingPoint(125, 50, 7010, 1); // Should be 1
    int global_index_2 = ecl_grid_reader_->GlobalIndexOfCellEnvelopingPoint(700, 800, 7100, 0); // Should be inside grid
    int global_index_3 = ecl_grid_reader_->GlobalIndexOfCellEnvelopingPoint(0, 0, 0, 0); // Should be outside grid
    EXPECT_EQ(global_index_1, 1);
    EXPECT_GT(global_index_2, -1);
    EXPECT_EQ(global_index_3, -1);
}

TEST_F(ECLGridReaderTest, BoundingCentroids) {
    ECLGridReader *norne_reader = new ECLGridReader();
    norne_reader->ReadEclGrid(norne_file_name_);
    auto bounding_centroids = norne_reader->GetBoundingCellCentroids();
    cout << endl;
    for (auto centroid : bounding_centroids) {
        cout << boost::lexical_cast<string>(centroid.x())
             << ", " << boost::lexical_cast<string>(centroid.y())
             << ", " << boost::lexical_cast<string>(centroid.z()) << endl;
    }
}

}
