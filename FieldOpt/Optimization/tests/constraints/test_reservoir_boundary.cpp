/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>
   Additions by M.Bellout (2017) <mathias.bellout@ntnu.no>

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
#include "constraints/reservoir_boundary.h"
#include "Optimization/tests/test_resource_cases.h"
#include "Reservoir/tests/test_resource_grids.h"
#include "Settings/tests/test_resource_settings.hpp"


namespace {

class ReservoirBoundaryTest : public ::testing::Test,
                              public TestResources::TestResourceCases,
                              public TestResources::TestResourceGrids,
                              public TestResources::TestResourceSettings {

 public:
  ReservoirBoundaryTest() {
      bound_settings_.type = Utilities::Settings::Optimizer::ConstraintType::ReservoirBoundary;
      bound_settings_.well = settings_optimizer_->constraints()[5].wells[1]; // TESTW
      bound_settings_.box_imin = settings_optimizer_->constraints()[5].box_imin;
      bound_settings_.box_imax = settings_optimizer_->constraints()[5].box_imax;
      bound_settings_.box_jmin = settings_optimizer_->constraints()[5].box_jmin;
      bound_settings_.box_jmax = settings_optimizer_->constraints()[5].box_jmax;
      bound_settings_.box_kmin = settings_optimizer_->constraints()[5].box_kmin;
      bound_settings_.box_kmax = settings_optimizer_->constraints()[5].box_kmax;

      boundary_constraint_ = new Optimization::Constraints::ReservoirBoundary(
          bound_settings_, variable_property_container_, grid_5spot_);
  }

  Utilities::Settings::Optimizer::Constraint bound_settings_;
  Optimization::Constraints::ReservoirBoundary *boundary_constraint_;

  virtual ~ReservoirBoundaryTest() { }
  virtual void TearDown() { }
  virtual void SetUp() { }
};

TEST_F(ReservoirBoundaryTest, Initialization) {
    // Replace test_boundary object with boundary_constraint_
    auto test_boundary = Optimization::Constraints::ReservoirBoundary(
        constraint_settings_reservoir_boundary_,
        varcont_prod_spline_, grid_5spot_);
    EXPECT_FALSE(test_boundary.CaseSatisfiesConstraint(test_case_spline_));
    test_boundary.SnapCaseToConstraints(test_case_spline_);
    EXPECT_TRUE(test_boundary.CaseSatisfiesConstraint(test_case_spline_));
}

TEST_F(ReservoirBoundaryTest, CheckListBoxEdgeCells) {
    auto box_edge_cells_ = boundary_constraint_->returnListOfBoxEdgeCellIndices();

    std::cout << "length box_edge_cells_: "
              << box_edge_cells_.length() << std::endl;

    // Keep this for now (will be part of further unit test)
    //for( int i=0; i < box_edge_cells_.length(); ++i )
    //{
    //   std::cout << box_edge_cells_[i] << std::endl;
    //}
    EXPECT_TRUE(box_edge_cells_.length()>0);
}

TEST_F(ReservoirBoundaryTest, CheckBoundConstraintSettings) {
    // std::cout << "i=" << i << " j=" << j << " k=" << k << std::endl;
    // std::cout << grid_->GetCell(i, j, k).global_index() << std::endl;

    boundary_constraint_->findCornerCells();
}
}
