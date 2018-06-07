/******************************************************************************
   Created by einar on 4/11/18.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "Settings/trajectory_importer.h"

namespace {


class TrajectoryImporterTest : public ::testing::Test {
 protected:
  TrajectoryImporterTest() { }
  virtual ~TrajectoryImporterTest() {}

  virtual void SetUp() {}
  virtual void TearDown() {}

  std::string trajectory_path_ = "../examples/ECLIPSE/norne-simplified/trajectories";
  std::vector<std::string> well_names_ = {"D-2H"};
};

TEST_F(TrajectoryImporterTest, Initialization) {
    auto importer = Settings::TrajectoryImporter(trajectory_path_, well_names_);
}

TEST_F(TrajectoryImporterTest, ImportedWell) {
    auto importer = Settings::TrajectoryImporter(trajectory_path_, well_names_);
    EXPECT_TRUE(importer.ContainsTrajectory("D-2H"));
    auto d_2h_traj = importer.GetImportedTrajectory("D-2H");
    EXPECT_EQ(35, d_2h_traj.size());

    EXPECT_EQ(13, d_2h_traj[0].ijk().x());
    EXPECT_EQ(11, d_2h_traj[0].ijk().y());
    EXPECT_EQ(21, d_2h_traj[0].ijk().z());
    EXPECT_FLOAT_EQ(455680.928, d_2h_traj[0].in().x());
    EXPECT_FLOAT_EQ(7320521.27, d_2h_traj[0].in().y());
    EXPECT_FLOAT_EQ(2766.89665, d_2h_traj[0].in().z());
    EXPECT_FLOAT_EQ(455680.907, d_2h_traj[0].out().x());
    EXPECT_FLOAT_EQ(7320521.26, d_2h_traj[0].out().y());
    EXPECT_FLOAT_EQ(2766.89985, d_2h_traj[0].out().z());


    EXPECT_EQ(15, d_2h_traj[d_2h_traj.size()-1].ijk().x());
    EXPECT_EQ(14, d_2h_traj[d_2h_traj.size()-1].ijk().y());
    EXPECT_EQ(10, d_2h_traj[d_2h_traj.size()-1].ijk().z());
}

}
