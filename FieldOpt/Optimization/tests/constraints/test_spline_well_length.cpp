/******************************************************************************
   Created by einar on 5/24/17.
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
#include "constraints/well_spline_length.h"
#include "Optimization/tests/test_resource_cases.h"
#include "Optimization/tests/test_resource_optimizer.h"

namespace {

class WellSplineLengthTest : public ::testing::Test,
                              public TestResources::TestResourceCases,
                              public TestResources::TestResourceSettings
{

 public:
  WellSplineLengthTest() {
      wsl_settings_.type = Settings::Optimizer::ConstraintType::WellSplineLength;
      wsl_settings_.min = 10;
      wsl_settings_.max = 100;
      wsl_settings_.well = "INJE";
      wsl_constraint_ = new Optimization::Constraints::WellSplineLength(wsl_settings_, varcont_two_spline_wells_);

      Eigen::Vector3d heel(25, 5, 200);
      Eigen::Vector3d toe(32, 120.1, 300);
      well_length_ = (heel - toe).norm();
  }

  Settings::Optimizer::Constraint wsl_settings_;
  Optimization::Constraints::WellSplineLength *wsl_constraint_;
  Optimization::Case *test_case = test_case_two_well_splines_;
  double well_length_;

  virtual ~WellSplineLengthTest() {}
  virtual void TearDown() {}
  virtual void SetUp() {}
};

TEST_F(WellSplineLengthTest, Penalty) {
    cout << "Actual length: " << well_length_ << endl;
    double penalty = wsl_constraint_->Penalty(test_case);
    EXPECT_NEAR(52.6336, penalty, 0.001); // Too long

    wsl_settings_.min = 175;
    wsl_settings_.max = 200;
    wsl_constraint_ = new Optimization::Constraints::WellSplineLength(wsl_settings_, varcont_two_spline_wells_);
    penalty = wsl_constraint_->Penalty(test_case);
    EXPECT_NEAR(22.3664, penalty, 0.001); // Too short

    wsl_settings_.min = 100;
    wsl_settings_.max = 200;
    wsl_constraint_ = new Optimization::Constraints::WellSplineLength(wsl_settings_, varcont_two_spline_wells_);
    penalty = wsl_constraint_->Penalty(test_case);
    EXPECT_EQ(0, penalty); // Just right
}

TEST_F(WellSplineLengthTest, Normalization) {
    double penalty = wsl_constraint_->Penalty(test_case);
    wsl_constraint_->InitializeNormalizer(QList<Optimization::Case *>({test_case}));
    long double penalty_norm = wsl_constraint_->PenaltyNormalized(test_case);
    EXPECT_NEAR(0.24766, penalty_norm, 0.005);
}

}
