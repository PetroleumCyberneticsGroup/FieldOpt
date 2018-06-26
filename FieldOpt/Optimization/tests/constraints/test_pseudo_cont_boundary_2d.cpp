/******************************************************************************
   Created by einar on 8/16/17.
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
#include "constraints/pseudo_cont_boundary_2d.h"
#include "Optimization/tests/test_resource_cases.h"
#include "Reservoir/tests/test_resource_grids.h"
#include "Optimization/tests/test_resource_optimizer.h"
#include "Model/tests/test_resource_variable_property_container.h"

namespace {
class PseudoContBoundary2DTest : public ::testing::Test,
                              public TestResources::TestResourceCases,
                              public TestResources::TestResourceGrids,
                              public TestResources::TestResourceSettings {

 public:
  PseudoContBoundary2DTest() {
      bound_settings_.type = Settings::Optimizer::ConstraintType::PseudoContBoundary2D;
      bound_settings_.well = "TESTW";
  }

  Settings::Optimizer::Constraint bound_settings_;
  Optimization::Constraints::PseudoContBoundary2D *pseudocont_constraint_;

  virtual ~PseudoContBoundary2DTest() { }
  virtual void TearDown() { }
  virtual void SetUp() { }
};

TEST_F(PseudoContBoundary2DTest, Initialization) {
    bound_settings_.box_imin = 0;
    bound_settings_.box_imax = 29;
    bound_settings_.box_jmin = 0;
    bound_settings_.box_jmax = 29;
    pseudocont_constraint_ =
        new Optimization::Constraints::PseudoContBoundary2D(
        bound_settings_, varcont_pseudocont_, grid_5spot_);
}

}
