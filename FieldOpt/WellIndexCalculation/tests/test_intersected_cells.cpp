/******************************************************************************
   Copyright (C) 2015-2016 Hilmar M. Magnusson <hilmarmag@gmail.com>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>
   Modified by Einar Baumann (2017) <einar.baumann@gmail.com>
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
#include "WellIndexCalculation/wellindexcalculator.h"

using namespace Reservoir::Grid;
using namespace Reservoir::WellIndexCalculation;
using namespace std;

namespace {

class IntersectedCellsTest : public ::testing::Test {
 protected:
  IntersectedCellsTest() {
      grid_ = new ECLGrid(file_path_);
      wic_ = WellIndexCalculator(grid_);
  }

  virtual ~IntersectedCellsTest() {
      delete grid_;
  }

  virtual void SetUp() {
  }

  virtual void TearDown() { }

  Grid *grid_;
  string file_path_ = "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
  WellIndexCalculator wic_;
};

TEST_F(IntersectedCellsTest, find_point_test) {
    cout << "find exit point test" << endl;

    // Load grid and chose first cell (cell 1,1,1)
    auto cell_1 = grid_->GetCell(0);
    // cout << "-load grid and chose first cell (cell 1,1,1)" << endl;
    //auto ptr_cell_1 = &cell_1;

    Eigen::Vector3d start_point = Eigen::Vector3d(0,0,1712);
    Eigen::Vector3d end_point = Eigen::Vector3d(25,25,1712);
    Eigen::Vector3d exit_point = Eigen::Vector3d(24,24,1712);
    // cout << "-define start_point, end_point, exit_point" << endl; // debug

    vector<WellDefinition> wells;
    // cout << "-define well" << endl; // debug
    wells.push_back(WellDefinition());
    wells.at(0).heels.push_back(start_point);
    wells.at(0).toes.push_back(end_point);
    wells.at(0).radii.push_back(0.190);
    wells.at(0).skins.push_back(0.0);
    wells.at(0).wellname = "testwell";

    map<string, vector<IntersectedCell>> cells;
    wic_.ComputeWellBlocks(cells, wells);

    vector<IntersectedCell> intersected_cell;
    int index_cell1 = IntersectedCell::GetIntersectedCellIndex(
        intersected_cell, cell_1);
    Eigen::Vector3d calc_exit_point = wic_.find_exit_point(
        intersected_cell, index_cell1,
        start_point, end_point, start_point);

    bool dir = (calc_exit_point - start_point).dot(end_point - start_point);
    if ( dir <= 0 ) {
        cout << "exit point wrong direction, try other direction" << endl;

        calc_exit_point = wic_.find_exit_point(
            intersected_cell, index_cell1,
            start_point, end_point, calc_exit_point);
        cout << "new algorithm exit point = "
             << calc_exit_point.x() << ","
             << calc_exit_point.y() << ","
             << calc_exit_point.z() << endl;
    }

    cout << "algorithm exit point = "
         << calc_exit_point.x() << ","
         << calc_exit_point.y() << ","
         << calc_exit_point.z() << endl;
    cout << "actual exit point = "
         << exit_point.x() << ","
         << exit_point.y() << ","
         << exit_point.z() << endl;
    double diff = (exit_point - calc_exit_point).norm();

    EXPECT_TRUE(diff<10-8);
}

TEST_F(IntersectedCellsTest, intersected_cell_test_cases) {

    // Load grid and chose first cell (cell 1,1,1)
    auto cell_0 = grid_->GetCell(0);
    //auto ptr_cell_1 = &cell_1;
    Eigen::Vector3d start_point = Eigen::Vector3d(0,0,1702);
    Eigen::Vector3d end_point = Eigen::Vector3d(44,84,1720);

    vector<WellDefinition> wells;
    wells.push_back(WellDefinition());
    wells.at(0).heels.push_back(start_point);
    wells.at(0).toes.push_back(end_point);
    wells.at(0).radii.push_back(0.190);
    wells.at(0).skins.push_back(0.0);
    wells.at(0).wellname = "testwell";

    map<string, vector<IntersectedCell>> cells;
    wic_.ComputeWellBlocks(cells, wells);

    // obsolete?
    // wic_.collect_intersected_cells(cells, start_point, end_point, 0.0190, std::vector<int>());

    cout << "number of cells intersected = " << cells.size() << endl;
    for( int ii = 0; ii< cells["testwell"].size(); ii++) {
        cout << "cell intersection number "
             << ii+1 << " with index number "
             << cells["testwell"][ii].global_index() << endl;

        cout << "line enters in point "
             << cells["testwell"][ii].get_segment_entry_point(0).x() << ","
             << cells["testwell"][ii].get_segment_entry_point(0).y() << ","
             << cells["testwell"][ii].get_segment_entry_point(0).z() << endl;
    }
}

TEST_F(IntersectedCellsTest, point_inside_cell_test) {

    // Load grid and chose first cell (cell 1,1,1)
    auto cell_0 = grid_->GetCell(0);
    auto cell_1 = grid_->GetCell(1);
    auto cell_60 = grid_->GetCell(60);

    Eigen::Vector3d point_0 = Eigen::Vector3d(0,0,1700);
    Eigen::Vector3d point_1 = Eigen::Vector3d(12,12,1712);
    Eigen::Vector3d point_2 = Eigen::Vector3d(24,12,1712);

    EXPECT_TRUE(cell_0.EnvelopsPoint(point_0));
    EXPECT_FALSE(cell_1.EnvelopsPoint(point_0));
    EXPECT_TRUE(cell_0.EnvelopsPoint(point_1));
    EXPECT_FALSE(cell_1.EnvelopsPoint(point_1));
    EXPECT_FALSE(cell_60.EnvelopsPoint(point_0));
    EXPECT_TRUE(cell_1.EnvelopsPoint(point_2));
}

//TEST_F(IntersectedCellsTest, ProblematicPathA) {
//
//    // Load grid and chose first cell (cell 1,1,1)
//    auto cell_0 = grid_->GetCell(0);
//    //auto ptr_cell_1 = &cell_1;
//    Eigen::Vector3d start_point = Eigen::Vector3d(290.0859, 1168.6483, 1711.5059);
//    Eigen::Vector3d end_point = Eigen::Vector3d(1113.9993,  107.1271, 1698.8978);
//
//    vector<WellDefinition> wells;
//    wells.push_back(WellDefinition());
//    wells.at(0).heels.push_back(start_point);
//    wells.at(0).toes.push_back(end_point);
//    wells.at(0).radii.push_back(0.190);
//    wells.at(0).skins.push_back(0.0);
//    wells.at(0).wellname = "testwell";
//
//    auto cells = wic_.ComputeWellBlocks(wells);
//    EXPECT_GT(cells["testwell"].size(), 1);
//}

//TEST_F(IntersectedCellsTest, ProblematicPathB) {
//
//  // Load grid and chose first cell (cell 1,1,1)
//  auto cell_0 = grid_->GetCell(0);
//  //auto ptr_cell_1 = &cell_1;
//  Eigen::Vector3d start_point = Eigen::Vector3d(-290.0859, 1168.6483, 1711.5059);
//  Eigen::Vector3d end_point = Eigen::Vector3d(1113.9993,  107.1271, 1698.8978);
//
//  vector<WellDefinition> wells;
//  wells.push_back(WellDefinition());
//  wells.at(0).heels.push_back(start_point);
//  wells.at(0).toes.push_back(end_point);
//  wells.at(0).radii.push_back(0.190);
//  wells.at(0).skins.push_back(0.0);
//  wells.at(0).wellname = "testwell";
//
//  auto cells = wic_.ComputeWellBlocks(wells);
//  EXPECT_GT(cells["testwell"].size(), 1);
//}

//TEST_F(IntersectedCellsTest, ProblematicPathC) {
//
//  // Load grid and chose first cell (cell 1,1,1)
//  auto cell_0 = grid_->GetCell(0);
//  //auto ptr_cell_1 = &cell_1;
//  Eigen::Vector3d start_point = Eigen::Vector3d(-213.3078, 1168.6483, 1711.5059);
//  Eigen::Vector3d end_point = Eigen::Vector3d(1107.0699, -430.7922, 1698.8978);
//
//  vector<WellDefinition> wells;
//  wells.push_back(WellDefinition());
//  wells.at(0).heels.push_back(start_point);
//  wells.at(0).toes.push_back(end_point);
//  wells.at(0).radii.push_back(0.190);
//  wells.at(0).skins.push_back(0.0);
//  wells.at(0).wellname = "testwell";
//
//  auto cells = wic_.ComputeWellBlocks(wells);
//  EXPECT_GT(cells["testwell"].size(), 1);
//}

//TEST_F(IntersectedCellsTest, ProblematicPathD) {
//
//  // Load grid and chose first cell (cell 1,1,1)
//  auto cell_0 = grid_->GetCell(0);
//  //auto ptr_cell_1 = &cell_1;
//  Eigen::Vector3d start_point = Eigen::Vector3d(213.3078, 1168.6483, 1711.5059);
//  Eigen::Vector3d end_point = Eigen::Vector3d(1107.0699,  430.7922, 1698.8978);
//
//  vector<WellDefinition> wells;
//  wells.push_back(WellDefinition());
//  wells.at(0).heels.push_back(start_point);
//  wells.at(0).toes.push_back(end_point);
//  wells.at(0).radii.push_back(0.190);
//  wells.at(0).skins.push_back(0.0);
//  wells.at(0).wellname = "testwell";
//
//  auto cells = wic_.ComputeWellBlocks(wells);
//  EXPECT_GT(cells["testwell"].size(), 1);
//}

//TEST_F(IntersectedCellsTest, ProblematicNornePathA) {
//  auto grid =  new ECLGrid("../examples/Flow/norne/NORNE_ATW2013.EGRID");
//  auto wic = WellIndexCalculator(grid);
//
//  Eigen::Vector3d start_point = Eigen::Vector3d(457518.693543, 7321524.904422, 2664.251818);
//  Eigen::Vector3d end_point = Eigen::Vector3d(458527.898804, 7321303.255945, 2674.991558);
//
//  vector<WellDefinition> wells;
//  wells.push_back(WellDefinition());
//  wells.at(0).heels.push_back(start_point);
//  wells.at(0).toes.push_back(end_point);
//  wells.at(0).radii.push_back(0.190);
//  wells.at(0).skins.push_back(0.0);
//  wells.at(0).wellname = "testwell";
//
//  auto cells = wic.ComputeWellBlocks(wells);
//  EXPECT_GT(cells["testwell"].size(), 2);
//}

//TEST_F(IntersectedCellsTest, ProblematicNornePathB) {
//  auto grid =  new ECLGrid("../examples/Flow/norne/NORNE_ATW2013.EGRID");
//  auto wic = WellIndexCalculator(grid);
//
//  Eigen::Vector3d start_point = Eigen::Vector3d(457695.839006, 7321303.496056, 2648.127423);
//  Eigen::Vector3d end_point = Eigen::Vector3d(458691.849848, 7321789.704993, 2687.949222);
//
//  vector<WellDefinition> wells;
//  wells.push_back(WellDefinition());
//  wells.at(0).heels.push_back(start_point);
//  wells.at(0).toes.push_back(end_point);
//  wells.at(0).radii.push_back(0.190);
//  wells.at(0).skins.push_back(0.0);
//  wells.at(0).wellname = "testwell";
//
//  auto cells = wic.ComputeWellBlocks(wells);
//  EXPECT_GT(cells["testwell"].size(), 2);
//}

//TEST_F(IntersectedCellsTest, ProblematicNornePathC) {
//  auto grid =  new ECLGrid("../examples/Flow/norne/NORNE_ATW2013.EGRID");
//  auto wic = WellIndexCalculator(grid);
//
//  Eigen::Vector3d start_point = Eigen::Vector3d(456020.932317, 7321189.825412, 2659.488457);
//  Eigen::Vector3d end_point = Eigen::Vector3d(458309.715382, 7321309.656581, 2649.867054);
//
//  vector<WellDefinition> wells;
//  wells.push_back(WellDefinition());
//  wells.at(0).heels.push_back(start_point);
//  wells.at(0).toes.push_back(end_point);
//  wells.at(0).radii.push_back(0.190);
//  wells.at(0).skins.push_back(0.0);
//  wells.at(0).wellname = "testwell";
//
//  auto cells = wic.ComputeWellBlocks(wells);
//  EXPECT_GT(cells["testwell"].size(), 2);
//}

//TEST_F(IntersectedCellsTest, ProblematicNornePathD) {
//  auto grid =  new ECLGrid("../examples/Flow/norne/NORNE_ATW2013.EGRID");
//  auto wic = WellIndexCalculator(grid);
//
//  Eigen::Vector3d start_point = Eigen::Vector3d(458589.704195, 7321218.390670, 2619.012866);
//  Eigen::Vector3d end_point = Eigen::Vector3d(456185.984986, 7322117.937520, 2691.531583);
//
//  vector<WellDefinition> wells;
//  wells.push_back(WellDefinition());
//  wells.at(0).heels.push_back(start_point);
//  wells.at(0).toes.push_back(end_point);
//  wells.at(0).radii.push_back(0.190);
//  wells.at(0).skins.push_back(0.0);
//  wells.at(0).wellname = "testwell";
//
//  auto cells = wic.ComputeWellBlocks(wells);
//  EXPECT_GT(cells["testwell"].size(), 2);
//}

//TEST_F(IntersectedCellsTest, ProblematicNornePathE) {
//  auto grid =  new ECLGrid("../examples/Flow/norne/NORNE_ATW2013.EGRID");
//  auto wic = WellIndexCalculator(grid);
//
//  Eigen::Vector3d start_point = Eigen::Vector3d(457916.07625384547, 7321300.9202301782, 2647.4969520910727);
//  Eigen::Vector3d end_point = Eigen::Vector3d(458726.16052064049, 7322019.2765819589, 2668.4723783396412);
//
//  vector<WellDefinition> wells;
//  wells.push_back(WellDefinition());
//  wells.at(0).heels.push_back(start_point);
//  wells.at(0).toes.push_back(end_point);
//  wells.at(0).radii.push_back(0.190);
//  wells.at(0).skins.push_back(0.0);
//  wells.at(0).wellname = "testwell";
//
//  auto cells = wic.ComputeWellBlocks(wells);
//  EXPECT_GT(cells["testwell"].size(), 2);
//}

//TEST_F(IntersectedCellsTest, ProblematicNornePathF) {
//  auto grid =  new ECLGrid("../examples/Flow/norne/NORNE_ATW2013.EGRID");
//  auto wic = WellIndexCalculator(grid);
//
//  Eigen::Vector3d start_point = Eigen::Vector3d(457557.48809118319, 7321482.4243003726, 2678.7179379941103);
//  Eigen::Vector3d end_point = Eigen::Vector3d(457783.4811529119, 7321171.8443912724, 2670.7274579296886);
//
//  vector<WellDefinition> wells;
//  wells.push_back(WellDefinition());
//  wells.at(0).heels.push_back(start_point);
//  wells.at(0).toes.push_back(end_point);
//  wells.at(0).radii.push_back(0.190);
//  wells.at(0).skins.push_back(0.0);
//  wells.at(0).wellname = "testwell";
//
//  auto cells = wic.ComputeWellBlocks(wells);
//  EXPECT_GT(cells["testwell"].size(), 2);
//}

//TEST_F(IntersectedCellsTest, ProblematicNornePathG) {
//    auto grid =  new ECLGrid("../examples/Flow/norne/OUTPUT/NORNE_ATW2013.EGRID");
//    auto wic = WellIndexCalculator(grid);
//
//    Eigen::Vector3d start_point = Eigen::Vector3d(457271.40395813627, 7321296.3664021967, 2612.345947265625);
//    Eigen::Vector3d end_point = Eigen::Vector3d(458760.19236537709, 7321748.8851311458, 2646.4210547072985);
//
//    vector<WellDefinition> wells;
//    wells.push_back(WellDefinition());
//    wells.at(0).heels.push_back(start_point);
//    wells.at(0).toes.push_back(end_point);
//    wells.at(0).radii.push_back(0.190);
//    wells.at(0).skins.push_back(0.0);
//    wells.at(0).wellname = "testwell";
//
//    auto cells = wic.ComputeWellBlocks(wells);
//    EXPECT_GT(cells["testwell"].size(), 2);
//}

TEST_F(IntersectedCellsTest, BoundingBox) {
    auto grid =  new ECLGrid("../examples/ECLIPSE/Brugge_xyz/BRUGGE.EGRID");
    auto wic = WellIndexCalculator(grid);

    double pertx = 0, perty = 0, pertz = 0;

    Eigen::Vector3d start_point = Eigen::Vector3d(
        -1.2611111755371094e+03 + pertx,
        1.2046807342529297e+04 + perty,
        1.5608086395263672e+03 + pertz);
    Eigen::Vector3d end_point = Eigen::Vector3d(
        -1.2491988525390625e+03 + pertx,
        1.2051784027099609e+04 + perty,
        1.5664830627441406e+03 + pertz);

    vector<WellDefinition> wells;
    wells.push_back(WellDefinition());
    wells.at(0).heels.push_back(start_point);
    wells.at(0).toes.push_back(end_point);
    wells.at(0).radii.push_back(0.1095);
    wells.at(0).skins.push_back(0.0);
    wells.at(0).wellname = "PROD";

    map<string, vector<IntersectedCell>> cells;
    wic_.ComputeWellBlocks(cells, wells);

}

}
