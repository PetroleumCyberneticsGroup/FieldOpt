/******************************************************************************
   Created by einar on 9/7/18.
   Copyright (C) 2018 Einar J.M. Baumann <einar.baumann@gmail.com>

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

namespace {


class TrajectoryCurveTest : public ::testing::Test,
                            public TestResources::TestResourceModel,
                            public TestResources::TestResourceGrids
 {
 protected:
  TrajectoryCurveTest() {
  }

};

TEST_F(TrajectoryCurveTest, SimpleBezier) {
  Curve* curve = new Bezier();
  curve->set_steps(100); // generate 100 interpolate points between the last 4 way points

  curve->add_way_point(Vector(1, 1, 0));
  curve->add_way_point(Vector(2, 3, 0));
  curve->add_way_point(Vector(3, 2, 0));
  curve->add_way_point(Vector(4, 6, 0));

//  std::cout << "nodes: " << curve->node_count() << std::endl;
//  std::cout << "total length: " << curve->total_length() << std::endl;
//  for (int i = 0; i < curve->node_count(); ++i) {
//    std::cout <<  curve->node(i).toString() << "," << std::endl;
//  }
  delete curve;

}
TEST_F(TrajectoryCurveTest, WellApplication) {
  Paths paths;
  paths.SetPath(Paths::GRID_FILE, TestResources::ExampleFilePaths::grid_5spot_);
  auto settings = Settings::Model(TestResources::TestResourceModelSettingSnippets::model_bezier_well(), paths);
  auto wsettings = settings.wells()[0];
  wsettings.use_bezier_spline = true;
  auto varcont = new Model::Properties::VariablePropertyContainer();
  auto well = Model::Wells::Wellbore::WellSpline(wsettings, varcont, TestResources::TestResourceGrids::grid_5spot_, nullptr);
  auto well_blocks = well.GetWellBlocks();

//  cout << "Well blocks:" << endl;
//  for (auto wb : *well_blocks) {
//    cout << "[" << wb->i() << ", " << wb->j() << "]," << endl;
//  }

  EXPECT_EQ(well_blocks->size(), 46);
  EXPECT_EQ(well_blocks->first()->i(), 13);
  EXPECT_EQ(well_blocks->first()->j(), 38);
  EXPECT_EQ(well_blocks->last()->i(), 38);
  EXPECT_EQ(well_blocks->last()->j(), 38);
}


}
