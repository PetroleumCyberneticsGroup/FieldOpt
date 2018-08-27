/******************************************************************************
   Copyright (C) 2015-2016 Hilmar M. Magnusson <hilmarmag@gmail.com>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>
   Modified by Einar Baumann (2017) <einar.bamann@gmail.com>
   Modified by Mathias Bellout (2017) <mathias.bellout@ntnu.no, chakibbb@gmail.com>

   This file and the WellIndexCalculator as a whole is part of the
   FieldOpt project. However, unlike the rest of FieldOpt, the
   WellIndexCalculator is provided under the GNU Lesser General Public
   License.

   WellIndexCalculator is free software: you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation, either version 3 of
   the License, or (at your option) any later version.

   WellIndexCalculator is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with WellIndexCalculator.  If not, see
   <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <gtest/gtest.h>
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "WellIndexCalculation/wicalc_rixx.h"

using namespace Reservoir::Grid;
using namespace Reservoir::WellIndexCalculation;

namespace {

class SingleCellWellIndexTest : public ::testing::Test {
 protected:
  SingleCellWellIndexTest() {
      grid_ = new ECLGrid(file_path_);
  }

  virtual ~SingleCellWellIndexTest() {
      delete grid_;
  }

  virtual void SetUp() {
  }

  virtual void TearDown() { }

  Grid *grid_;
  std::string file_path_ = "../examples/ECLIPSE/cube_9x9/CUBE.EGRID";


  WellDefinition init_well(Eigen::Vector3d start_point, Eigen::Vector3d end_point) {
      WellDefinition well;
      well.heels.push_back(start_point);
      well.toes.push_back(end_point);
      well.radii.push_back(0.1905/2.0);
      well.skins.push_back(0.0);
      well.wellname = "testwell";
      well.heel_md.push_back(0.0);
      well.toe_md.push_back((end_point - start_point).norm());
      return well;
  }

};

TEST_F(SingleCellWellIndexTest, WellIndexValueVertical) {
    // Use first cell (global index 0)
    //    auto cell_1 = grid_->GetCell(0);
    //double kx = 1.689380;
    //double ky = 1.689380;
    //double kz = 1;
    // dx, dy, dz = 24 m
    //Well spline: Let it go vertically through the centre of the block.
    Eigen::Vector3d start_point = Eigen::Vector3d(1.5, 1.5, 1700.00001);
    Eigen::Vector3d end_point   = Eigen::Vector3d(1.5, 1.5, 1702.99999);

    auto well = init_well(start_point, end_point);

    auto wic = wicalc_rixx(grid_);
    vector<IntersectedCell> well_indices;
    wic.ComputeWellBlocks(well_indices, well);
    auto wblocks = well_indices;

    EXPECT_EQ(wblocks.size(), 3);
    EXPECT_EQ(wblocks[1].global_index(), 13);
    EXPECT_FLOAT_EQ(wblocks[1].permx().back(), 1.0);
    EXPECT_FLOAT_EQ(wblocks[1].permy().back(), 1.0);
    EXPECT_FLOAT_EQ(wblocks[1].permz().back(), 1.0);

    //0.555602 is the expected well transmisibility factor aka. well index.
    //Expect value within delta percent.
    double delta = 0.001;
    double wi = wblocks[1].cell_well_index_matrix();
    EXPECT_NEAR(wi, 0.073221321, delta/100);
}
TEST_F(SingleCellWellIndexTest, WellIndexValueHorzX) {
    // Use first cell (global index 0)
    //    auto cell_1 = grid_->GetCell(0);
    //double kx = 1.689380;
    //double ky = 1.689380;
    //double kz = 1;
    // dx, dy, dz = 24 m
    //Well spline: Let it go vertically through the centre of the block.
    Eigen::Vector3d start_point = Eigen::Vector3d(1, 1.5, 1701.5);
    Eigen::Vector3d end_point   = Eigen::Vector3d(2, 1.5, 1701.5);

    auto well = init_well(start_point, end_point);

    auto wic = wicalc_rixx(grid_);
    vector<IntersectedCell> well_indices;
    wic.ComputeWellBlocks(well_indices, well);
    auto wblocks = well_indices;

    EXPECT_EQ(wblocks.size(), 1);
    EXPECT_EQ(wblocks[0].global_index(), 13);
    EXPECT_FLOAT_EQ(wblocks[0].permx().back(), 1.0);
    EXPECT_FLOAT_EQ(wblocks[0].permy().back(), 1.0);
    EXPECT_FLOAT_EQ(wblocks[0].permz().back(), 1.0);

    //0.555602 is the expected well transmisibility factor aka. well index.
    //Expect value within delta percent.
    double delta = 0.001;
    double wi = wblocks[0].cell_well_index_matrix();
    EXPECT_NEAR(wi, 0.073221321, delta/100);
}


TEST_F(SingleCellWellIndexTest, Well_index_grid_test) {
    file_path_ = "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
    grid_ = new ECLGrid(file_path_);
    double wellbore_radius = 0.191/2;

    Eigen::Vector3d start_point = Eigen::Vector3d(0.05,0.00,1712);
    Eigen::Vector3d end_point= Eigen::Vector3d(1440.0,1400.0,1712);
    std::vector<Eigen::Vector3d> well_spline_points = {start_point, end_point};

    // \todo The following lines need to be changed
    auto wic = wicalc_rixx(grid_);

    WellDefinition well;
    well.heels.push_back(start_point);
    well.toes.push_back(end_point);
    well.radii.push_back(wellbore_radius);
    well.skins.push_back(0.0);
    well.wellname = "testwell";
    well.heel_md.push_back(0.0);
    well.toe_md.push_back((end_point - start_point).norm());

    vector<IntersectedCell> well_indices;
    wic.ComputeWellBlocks(well_indices, well);
    auto wblocks = well_indices;

    EXPECT_EQ(117, wblocks.size());
}
}
