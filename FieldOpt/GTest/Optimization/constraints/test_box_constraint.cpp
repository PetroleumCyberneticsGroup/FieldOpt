/******************************************************************************
 *
 *
 *
 * Created: 03.12.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#include "GTest/Optimization/test_fixture_case.h"
#include "Optimization/constraints/box_constraint.h"

namespace {

class BoxConstraintTest : public CaseTestFixture {
protected:
    BoxConstraintTest() {
        box_constraint_ = new ::Optimization::Constraints::BoxConstraint(optimizer_settings_->constraints()->first(), model_->variables());
    }
    virtual ~BoxConstraintTest() {}
    virtual void SetUp() {}

    Optimization::Constraints::BoxConstraint *box_constraint_;
};

TEST_F(BoxConstraintTest, Constructor) {
    EXPECT_STREQ(box_constraint_->name().toLatin1().constData(), "PROD-BHP-1");
}

TEST_F(BoxConstraintTest, BaseCaseSatasfaction) {
    EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));
}

TEST_F(BoxConstraintTest, SatisfactionAfterModification) {
    // get the bhp variable ids
    QList<QUuid> bhp_ids = model_->variables()->GetContinousVariableIdsWithName("PROD-BHP-1");

    // set all bhps to 5000 (which exceeds the max value defined, 3000)
    foreach (QUuid key, bhp_ids) {
        base_case_->set_real_variable_value(key, 5000);
    }
    EXPECT_FALSE(box_constraint_->CaseSatisfiesConstraint(base_case_));

    // set all bhps to 100 (which is less than the min value defined, 1000)
    foreach (QUuid key, bhp_ids) {
        base_case_->set_real_variable_value(key, 100);
    }
    EXPECT_FALSE(box_constraint_->CaseSatisfiesConstraint(base_case_));

    // set all bhps to 1125 (which is within the defined bounds)
    foreach (QUuid key, bhp_ids) {
        base_case_->set_real_variable_value(key, 1125);
    }
    EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));
}

TEST_F(BoxConstraintTest, SnappingValues) {
    // get the bhp variable ids
    QList<QUuid> bhp_ids = model_->variables()->GetContinousVariableIdsWithName("PROD-BHP-1");

    // set all bhps to 5000 (which exceeds the max value defined, 3000)
    foreach (QUuid key, bhp_ids) {
        base_case_->set_real_variable_value(key, 5000);
    }
    EXPECT_FALSE(box_constraint_->CaseSatisfiesConstraint(base_case_));
    // Snap the values back and recheck
    box_constraint_->SnapCaseToConstraints(base_case_);
    EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));

    // set all bhps to 100 (which is less than the min value defined, 1000)
    foreach (QUuid key, bhp_ids) {
        base_case_->set_real_variable_value(key, 100);
    }
    EXPECT_FALSE(box_constraint_->CaseSatisfiesConstraint(base_case_));
    // Snap the values back and recheck
    box_constraint_->SnapCaseToConstraints(base_case_);
    EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));
}

}

