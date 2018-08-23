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
#include "FieldOpt-WellIndexCalculator/wellindexcalculator.h"

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
  std::string file_path_ = "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
};

TEST_F(SingleCellWellIndexTest, WellIndexValueWithQVector_test) {

    // Load grid and chose first icell (icell 1,1,1)
    //double kx = 1.689380;
    //double ky = 1.689380;
    //double kz = 1;
    //Figure out conversions and shit?
    double wellbore_radius = 0.1905/2;
    double skin_factor = 0.0;

    auto cell_1 = grid_->GetCell(0);
    auto ptr_cell_1 = cell_1;
    auto corners = cell_1.corners();

    //Determine well placement. Let it go vertically through the centre of the block.
    double well_start_x = 0.25*corners[0].x() + 0.25*corners[1].x() +0.25*corners[2].x() + 0.25*corners[3].x();
    double well_start_y = 0.25*corners[0].y() + 0.25*corners[1].y() +0.25*corners[2].y() + 0.25*corners[3].y();
    double well_start_z = 0.25*corners[0].z() + 0.25*corners[1].z() +0.25*corners[2].z() + 0.25*corners[3].z();

    double well_end_x = 0.25*corners[4].x() + 0.25*corners[5].x() +0.25*corners[6].x() + 0.25*corners[7].x();
    double well_end_y = 0.25*corners[4].y() + 0.25*corners[5].y() +0.25*corners[6].y() + 0.25*corners[7].y();
    double well_end_z = 0.25*corners[4].z() + 0.25*corners[5].z() +0.25*corners[6].z() + 0.25*corners[7].z();

    Eigen::Vector3d start_point = Eigen::Vector3d(well_start_x, well_start_y, well_start_z);
    Eigen::Vector3d end_point = Eigen::Vector3d(well_end_x, well_end_y, well_end_z);

    auto icell = IntersectedCell(cell_1);
    icell.add_new_segment(start_point, end_point, wellbore_radius, skin_factor);

    auto wic = WellIndexCalculator(grid_);

    std::vector<WellDefinition> wells;
    wells.push_back(WellDefinition());
    wells.at(0).heels.push_back(start_point);
    wells.at(0).toes.push_back(end_point);
    wells.at(0).radii.push_back(wellbore_radius);
    wells.at(0).skins.push_back(skin_factor);
    wells.at(0).wellname = "testwell";

    map<string, vector<IntersectedCell>> well_indices;
    wic.ComputeWellBlocks(well_indices, wells);
    auto wblocks = well_indices[wells.at(0).wellname];

    EXPECT_GT(wblocks.size(), 0);
    double wi = wblocks[0].cell_well_index_matrix();

    // \todo The function cell_well_index() should deal with the case when
    // the number of well blocks is zero (which results in segfault), e.g.,
    // throw an error
    //
    // Note: The error here comes from wblock[0] which does not exist...
    // cell_well_index() cannot be called here if there is no block

    /* 0.555602 is the expected well transmisibility factor aka. well index.
     * For now this value is read directly from eclipse output file:
     * Expect value within delta percent
     */
    double delta = 0.001;
    EXPECT_NEAR(wi, 0.555602, delta/100);
}

TEST_F(SingleCellWellIndexTest, vertical_well_index_test) {
    double wellbore_radius = 0.1905/2;
    double skin_factor = 0.0;

    auto cell_1 = grid_->GetCell(0);
    auto corners = cell_1.corners();

    //Determine well placement. Let it go vertically through the centre of the block.
    double well_start_x = 0.25*corners[0].x() + 0.25*corners[1].x() +0.25*corners[2].x() + 0.25*corners[3].x();
    double well_start_y = 0.25*corners[0].y() + 0.25*corners[1].y() +0.25*corners[2].y() + 0.25*corners[3].y();
    double well_start_z = 0.25*corners[0].z() + 0.25*corners[1].z() +0.25*corners[2].z() + 0.25*corners[3].z();

    double well_end_x = 0.25*corners[4].x() + 0.25*corners[5].x() +0.25*corners[6].x() + 0.25*corners[7].x();
    double well_end_y = 0.25*corners[4].y() + 0.25*corners[5].y() +0.25*corners[6].y() + 0.25*corners[7].y();
    double well_end_z = 0.25*corners[4].z() + 0.25*corners[5].z() +0.25*corners[6].z() + 0.25*corners[7].z();
    Eigen::Vector3d start_point = Eigen::Vector3d(well_start_x, well_start_y, well_start_z);
    Eigen::Vector3d end_point= Eigen::Vector3d(well_end_x,well_end_y, well_end_z);

    Reservoir::WellIndexCalculation::IntersectedCell icell(cell_1);
    icell.add_new_segment(start_point, end_point, wellbore_radius, skin_factor);

    auto wic = WellIndexCalculator(grid_);

    std::vector<WellDefinition> wells;
    wells.push_back(WellDefinition());
    wells.at(0).heels.push_back(start_point);
    wells.at(0).toes.push_back(end_point);
    wells.at(0).radii.push_back( wellbore_radius);
    wells.at(0).skins.push_back(0.0);
    wells.at(0).wellname = "testwell";

    map<string, vector<IntersectedCell>> well_indices;
    wic.ComputeWellBlocks(well_indices, wells);
    auto wblocks = well_indices[wells.at(0).wellname];

    EXPECT_GT(wblocks.size(), 0);
    double wi = wblocks[0].cell_well_index_matrix();

    /* 0.555602 is the expected well transmisibility factor aka. well index.
     * For now this value is read directly from eclipse output file:
     * Expect value within delta percent
     */
    double delta = 0.001;
    EXPECT_NEAR(wi, 0.555602, delta/100);
}

TEST_F(SingleCellWellIndexTest, Well_index_grid_test) {
    double wellbore_radius = 0.191/2;

    Eigen::Vector3d start_point = Eigen::Vector3d(0.05,0.00,1712);
    Eigen::Vector3d end_point= Eigen::Vector3d(1440.0,1400.0,1712);
    std::vector<Eigen::Vector3d> well_spline_points = {start_point, end_point};

    // \todo The following lines need to be changed
    auto wic = WellIndexCalculator(grid_);

    vector<WellDefinition> wells;
    wells.push_back(WellDefinition());
    wells.at(0).heels.push_back(start_point);
    wells.at(0).toes.push_back(end_point);
    wells.at(0).radii.push_back(wellbore_radius);
    wells.at(0).skins.push_back(0.0);
    wells.at(0).wellname = "testwell";

    map<string, vector<IntersectedCell>> well_indices;
    wic.ComputeWellBlocks(well_indices, wells);
    auto wblocks = well_indices[wells.at(0).wellname];

    EXPECT_EQ(118, wblocks.size());
}
}
