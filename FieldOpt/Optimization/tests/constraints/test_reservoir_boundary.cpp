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
  ReservoirBoundaryTest() { }
  virtual ~ReservoirBoundaryTest() { }
  virtual void TearDown() { }
  virtual void SetUp() { }
};

TEST_F(ReservoirBoundaryTest, Initialization) {
    auto test_boundary = Optimization::Constraints::ReservoirBoundary(
        constraint_settings_reservoir_boundary_,
        varcont_prod_spline_, grid_5spot_);
    EXPECT_FALSE(test_boundary.CaseSatisfiesConstraint(test_case_spline_));
    test_boundary.SnapCaseToConstraints(test_case_spline_);
    EXPECT_TRUE(test_boundary.CaseSatisfiesConstraint(test_case_spline_));
}

TEST_F(ReservoirBoundaryTest, CheckListBoxEdgeCells) {
    auto test_boundary = Optimization::Constraints::ReservoirBoundary(
        constraint_settings_reservoir_boundary_,
        variable_property_container_, grid_5spot_);

    auto box_edge_cells_ = test_boundary.returnListOfBoxEdgeCellIndices();
    std::cout << "length (test_reservoir_boundary.cpp):"
              << box_edge_cells_.length() << std::endl;

    // Keep this for now (will be part of further unit test)
    //for( int i=0; i < box_edge_cells_.length(); ++i ) {
    //    std::cout << box_edge_cells_[i] << std::endl;
    //}
    // std::cout << "i=" << i << " j=" << j << " k=" << k << std::endl;
    // std::cout << grid_->GetCell(i, j, k).global_index() << std::endl;

    EXPECT_TRUE(box_edge_cells_.length()>0);
}
}
