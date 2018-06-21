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

#include "gtest/gtest.h"
#include "Optimization/constraints/wcntrl_bhp.h"
#include "Settings/tests/test_resource_settings.hpp"
#include "Optimization/tests/test_resource_optimizer.h"

namespace {

class BhpConstraintTest : public ::testing::Test,
                          public TestResources::TestResourceOptimizer {
 protected:
  BhpConstraintTest() {
      box_constraint_ = new ::Optimization::Constraints::BhpConstraint
          (settings_optimizer_->constraints()[0], model_->variables());
  }
  virtual ~BhpConstraintTest() {}
  virtual void SetUp() {}

  Optimization::Constraints::BhpConstraint *box_constraint_;
};

TEST_F(BhpConstraintTest, Constructor) {
    EXPECT_TRUE(true);
}

TEST_F(BhpConstraintTest, BaseCaseSatisfaction) {
    EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));
}

TEST_F(BhpConstraintTest, SatisfactionAfterModification) {
    // get the bhp variable ids
    QList<QUuid> bhp_ids;
    for (auto var : model_->variables()->GetWellBHPVariables("PROD")) {
        bhp_ids.append(var->id());
    }

    // set all bhps to 500 (which exceeds the max value defined, 300)
    for (QUuid key : bhp_ids) {
        base_case_->set_real_variable_value(key, 500);
    }
    EXPECT_FALSE(box_constraint_->CaseSatisfiesConstraint(base_case_));

    // set all bhps to 50 (which is less than the min value defined, 100)
    for (QUuid key : bhp_ids) {
        base_case_->set_real_variable_value(key, 50);
    }
    EXPECT_FALSE(box_constraint_->CaseSatisfiesConstraint(base_case_));

    // set all bhps to 125 (which is within the defined bounds)
    for (QUuid key : bhp_ids) {
        base_case_->set_real_variable_value(key, 125);
    }
    EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));
}

TEST_F(BhpConstraintTest, SnappingValues) {
    // get the bhp variable ids
    QList<QUuid> bhp_ids;
    for (auto var : model_->variables()->GetWellBHPVariables("PROD")) {
        bhp_ids.append(var->id());
    }

    // set all bhps to 500 (which exceeds the max value defined, 300)
    for (QUuid key : bhp_ids) {
        base_case_->set_real_variable_value(key, 500);
    }
    EXPECT_FALSE(box_constraint_->CaseSatisfiesConstraint(base_case_));
    // Snap the values back and recheck
    box_constraint_->SnapCaseToConstraints(base_case_);
    EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));

    // set all bhps to 50 (which is less than the min value defined, 100)
    for (QUuid key : bhp_ids) {
        base_case_->set_real_variable_value(key, 50);
    }
    EXPECT_FALSE(box_constraint_->CaseSatisfiesConstraint(base_case_));
    // Snap the values back and recheck
    box_constraint_->SnapCaseToConstraints(base_case_);
    EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));
}

}

