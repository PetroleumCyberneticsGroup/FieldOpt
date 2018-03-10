/******************************************************************************
   Copyright (C) 2017 Mathias Bellout [Created on 20170731]
   <mathias.bellout@ntnu.no, chakibbb@gmail.com>

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
#include "ERTWrapper/eclgridrotator.h"

using namespace ERTWrapper::ECLGrid;
using namespace std;

namespace {

class ECLGridRotatorTest : public ::testing::Test {
 protected:
  ECLGridRotatorTest() {
      ecl_grid_rotator_horzwell_ = new ECLGridRotator();
      ecl_grid_rotator_brugge_ = new ECLGridRotator();
      ecl_grid_rotator_5spot_ = new ECLGridRotator();
      ecl_grid_rotator_5spot_exp_ = new ECLGridRotator();
  }

  virtual ~ECLGridRotatorTest() {
      delete ecl_grid_rotator_horzwell_;
      delete ecl_grid_rotator_brugge_;
      delete ecl_grid_rotator_5spot_;
      delete ecl_grid_rotator_5spot_exp_;
  }

  virtual void SetUp() {
      ecl_grid_rotator_horzwell_->ecl_grid_reader_->ReadEclGrid(grid_horzwell_);
      ecl_grid_rotator_brugge_->ecl_grid_reader_->ReadEclGrid(grid_brugge_);
      ecl_grid_rotator_5spot_->ecl_grid_reader_->ReadEclGrid(grid_5spot_);
      ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->ReadEclGrid(grid_5spot_exp_);
  }

  virtual void TearDown() {}

  ECLGridRotator *ecl_grid_rotator_horzwell_;
  ECLGridRotator *ecl_grid_rotator_brugge_;
  ECLGridRotator *ecl_grid_rotator_5spot_;
  ECLGridRotator *ecl_grid_rotator_5spot_exp_;

  // Simulation grid files
  string grid_horzwell_ = "../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID";
  string grid_brugge_ = "../examples/ECLIPSE/Brugge_xyz/BRUGGE.EGRID";
  string grid_5spot_ = "../examples/ECLIPSE/5spot/ECL_5SPOT.EGRID";
  string grid_5spot_exp_ = "../examples/ECLIPSE/5spot_exp/ECL_5SPOT_NOROT.EGRID";

};

TEST_F(ECLGridRotatorTest, SetRotationMatrix) {

    // Test experimental 5spot grid ------------------------------
    Matrix<double,3,1> rx_ry_rz;
    rx_ry_rz << 0.0, 0.0, 0.0;
    ecl_grid_rotator_5spot_exp_->SetRotationMatrix(rx_ry_rz);

    // Further tests later
    auto gidx_5spot_exp_ = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->gidx_;
    auto gridData_5spot_exp_ = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->gridData_;
}

TEST_F(ECLGridRotatorTest, TestCOORDAfterRotationSizes) {

    // Test experimental 5spot grid ------------------------------
    Matrix<double,3,1> rx_ry_rz;
    rx_ry_rz << 0.0, 0.0, 0.0;
    ecl_grid_rotator_5spot_exp_->SetRotationMatrix(rx_ry_rz);
    // ecl_grid_rotator_5spot_exp_->GetParametersFromJSON();

    // Check COORD data is nonzero
    cout << YELLOW << "== Check COORD data is nonzero ==" << END << endl;
    ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->GetCOORDZCORNData();
    EXPECT_EQ(22326, ecl_grid_rotator_5spot_exp_->
        ecl_grid_reader_->gridData_.coord.size());

    // Rotate grid
    cout << YELLOW << "== Rotate grid ==" << END << endl;
    ecl_grid_rotator_5spot_exp_->RotateCOORD();

    // Compare original COORD data againts rotated COORD data: should
    // be the same since rx = ry = rz = 0
    auto coord = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->gridData_.coord;
    auto coord_rxryrz = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->gridData_.coord_rxryrz;

    // Check data sizes
    cout << YELLOW << "== COORD.SIZE() ==" << END << endl;
    cout << "[coord.rows() coord.cols()]: ["
         << coord.rows() << " " << coord.cols() << "]" << endl;
    cout << "[coord_rxryrz.rows() coord_rxryrz.cols()]: ["
         << coord_rxryrz.rows() << " " << coord_rxryrz.cols() << "]" << endl;
    EXPECT_EQ(coord.size(), coord_rxryrz.size());

    // View COORD data (column format)
    cout.precision(2); cout.setf(ios::fixed, ios::floatfield);
    cout << YELLOW << "== COORD.DATA() ==" << END << endl;
    cout << "coord:" << endl << coord.block(0,0,9,1).transpose() << endl;
    cout << "coord_rxryrz:" << endl << coord_rxryrz.block(0,0,9,1).transpose() << endl;

    // Test equal values b/e original rotated COORD data
    EXPECT_TRUE((coord-coord_rxryrz).isZero(1e-1));
}

TEST_F(ECLGridRotatorTest, TestCOORDAfterRotationRx) {

    // Rotate grid using Rx
    Matrix<double,3,1> rx_ry_rz;
    rx_ry_rz << 0.1, 0.0, 0.0;
    ecl_grid_rotator_5spot_exp_->SetRotationMatrix(rx_ry_rz, true);
    ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->GetCOORDZCORNData();
    ecl_grid_rotator_5spot_exp_->RotateCOORD();

    // Extract original + rotated data (xyz format)
    auto coord_xyz = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->gridData_.coord_xyz;
    auto coord_rxryrz_xyz = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->gridData_.coord_rxryrz_xyz;

    // View COORD data (XYZ format)
    cout.precision(5); cout.setf(ios::fixed, ios::floatfield);
    cout << YELLOW << "== COORD.DATA().XYZ AFTER RX ROTATION ==" << END << endl;
    cout << "coord_xzy:" << endl << coord_xyz.block(0,0,9,3) << endl;
    cout << "coord_rxryrz_xzy:" << endl << coord_rxryrz_xyz.block(0,0,9,3) << endl;

    // X column should be equal after rotation
//    EXPECT_TRUE((coord_xyz.col(1)-coord_rxryrz_xyz.col(1)).isZero(1e-1));
    auto coord_diff = coord_xyz.col(0)-coord_rxryrz_xyz.col(0);
    cout << "coord_diff.size(): " << coord_diff.size() << endl;
    cout << "coord_diff.isZero(1e-1): " << coord_diff.isZero(1e-1) << endl;

//    EXPECT_TRUE( coord_diff.isZero(1e-1) );
}

TEST_F(ECLGridRotatorTest, TestCOORDAfterRotationRy) {

    // Rotate grid using Rx
    Matrix<double,3,1> rx_ry_rz;
    rx_ry_rz << 0.0, 0.1, 0.0;
    ecl_grid_rotator_5spot_exp_->SetRotationMatrix(rx_ry_rz, true);
    ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->GetCOORDZCORNData();
    ecl_grid_rotator_5spot_exp_->RotateCOORD();

    // Extract original + rotated data (xyz format)
    auto coord_xyz = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->gridData_.coord_xyz;
    auto coord_rxryrz_xyz = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->gridData_.coord_rxryrz_xyz;

    // View COORD data (XYZ format)
    cout.precision(5); cout.setf(ios::fixed, ios::floatfield);
    cout << YELLOW << "== COORD.DATA().XYZ AFTER RY ROTATION ==" << END << endl;
    cout << "coord_xzy:" << endl << coord_xyz.block(0,0,9,3) << endl;
    cout << "coord_rxryrz_xzy:" << endl << coord_rxryrz_xyz.block(0,0,9,3) << endl;

    // Y column should be equal after rotation
    auto coord_diff = coord_xyz.col(1)-coord_rxryrz_xyz.col(1);
    cout << "coord_diff.size(): " << coord_diff.size() << endl;
    cout << "coord_diff.isZero(1e-1): " << coord_diff.isZero(1e-1) << endl;
//    EXPECT_TRUE( coord_diff.isZero(1e-1) );
}


TEST_F(ECLGridRotatorTest, TestCOORDAfterRotationRz) {

    // Rotate grid using Rx
    Matrix<double,3,1> rx_ry_rz;
    rx_ry_rz << 0.0, 0.0, 0.1;
    ecl_grid_rotator_5spot_exp_->SetRotationMatrix(rx_ry_rz, true);
    ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->GetCOORDZCORNData();
    ecl_grid_rotator_5spot_exp_->RotateCOORD();

    // Extract original + rotated data (xyz format)
    auto coord_xyz = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->gridData_.coord_xyz;
    auto coord_rxryrz_xyz = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->gridData_.coord_rxryrz_xyz;

    // View COORD data (XYZ format)
    cout.precision(5); cout.setf(ios::fixed, ios::floatfield);
    cout << YELLOW << "== COORD.DATA().XYZ AFTER RZ ROTATION ==" << END << endl;
    cout << "coord_xzy:" << endl << coord_xyz.block(0,0,9,3) << endl;
    cout << "coord_rxryrz_xzy:" << endl << coord_rxryrz_xyz.block(0,0,9,3) << endl;

    // Z column should be equal after rotation
    auto coord_diff = coord_xyz.col(2)-coord_rxryrz_xyz.col(2);
    cout << "coord_diff.size(): " << coord_diff.size() << endl;
    cout << "coord_diff.isZero(1e-1): " << coord_diff.isZero(1e-1) << endl;

//    EXPECT_TRUE( coord_diff.isZero(1e-1) );
}

TEST_F(ECLGridRotatorTest, TestZCORNAfterRotation) {

    // Test experimental 5spot grid ------------------------------
    Matrix<double,3,1> rx_ry_rz;
    rx_ry_rz << 0.0, 0.0, 0.0;
    ecl_grid_rotator_5spot_exp_->SetRotationMatrix(rx_ry_rz);
    ecl_grid_rotator_5spot_exp_->ecl_grid_reader_->GetCOORDZCORNData();
    ecl_grid_rotator_5spot_exp_->RotateCOORD();

    bool rotate_zcorn = true;
    ecl_grid_rotator_5spot_exp_->RotateZCORN(rotate_zcorn);

    auto zcorn = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_
        ->gridData_.zcorn;
    auto zcorn_rxryrz = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_
        ->gridData_.zcorn_rxryrz;
    int n_total = ecl_grid_rotator_5spot_exp_->ecl_grid_reader_
        ->gidx_.n_total;

    // Check data sizes
    cout << "[zcorn.rows() zcorn.cols()]: ["
         << zcorn.rows() << " "
         << zcorn.cols() << "]" << endl;

    cout << "[zcorn.rows() zcorn.cols()]: ["
         << zcorn_rxryrz.rows() << " "
         << zcorn_rxryrz.cols() << "]" << endl;

    // View data
    int offset = n_total*4; // 14400
    cout << "original zcorn [offset=" << offset << "]:" << endl
         << zcorn.block(0,0,16,1).transpose() << endl
         << zcorn.block(offset,0,16,1).transpose() << endl;
    cout << "zcorn_rxryrz [offset=" << offset << "]:" << endl
         << zcorn_rxryrz.block(0,0,16,1).transpose() << endl
         << zcorn_rxryrz.block(offset,0,16,1).transpose() << endl;

}

}