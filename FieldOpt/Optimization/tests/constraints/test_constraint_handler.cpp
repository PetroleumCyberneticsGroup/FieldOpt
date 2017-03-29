/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "Optimization/tests/test_resource_optimizer.h"
#include "Optimization/constraints/constraint_handler.h"
#include "Reservoir/tests/test_resource_grids.h"

namespace {

class ConstraintHandlerTest : public ::testing::Test,
                              public TestResources::TestResourceOptimizer,
                              public TestResources::TestResourceGrids {
 protected:
  ConstraintHandlerTest() {
      constraint_handler_ = new ::Optimization::Constraints::ConstraintHandler(
          settings_optimizer_->constraints(),
          model_->variables(), grid_5spot_);
  }
  virtual ~ConstraintHandlerTest() {}
  virtual void SetUp() {}

  Optimization::Constraints::ConstraintHandler *constraint_handler_;
};

TEST_F(ConstraintHandlerTest, Constructor) {
    EXPECT_EQ(6, constraint_handler_->constraints().size());
}

TEST_F(ConstraintHandlerTest, BaseCaseSatisfaction) {
    EXPECT_TRUE(constraint_handler_->constraints().first()->CaseSatisfiesConstraint(base_case_));
}

// Check if base case breaks second constraint type (WellSplineLength)
TEST_F(ConstraintHandlerTest, BaseCaseSatisfyWellSplineLengthConstraint) {
    EXPECT_FALSE(constraint_handler_->constraints()[1]->CaseSatisfiesConstraint(base_case_));
}

}
