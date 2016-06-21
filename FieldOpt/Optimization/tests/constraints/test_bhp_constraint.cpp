#include "gtest/gtest.h"
#include "Optimization/constraints/bhp_constraint.h"
#include "Utilities/tests/test_resource_settings.h"
#include "Optimization/tests/test_resource_optimizer.h"

namespace {

    class BhpConstraintTest : public ::testing::Test, public TestResources::TestResourceOptimizer {
    protected:
        BhpConstraintTest() {
            box_constraint_ = new ::Optimization::Constraints::BhpConstraint(settings_optimizer_->constraints()[0], model_->variables());
        }
        virtual ~BhpConstraintTest() {}
        virtual void SetUp() {}

        Optimization::Constraints::BhpConstraint *box_constraint_;
    };

    TEST_F(BhpConstraintTest, Constructor) {
        EXPECT_TRUE(true);
    }

    TEST_F(BhpConstraintTest, BaseCaseSatasfaction) {
        EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));
    }

    TEST_F(BhpConstraintTest, SatisfactionAfterModification) {
        // get the bhp variable ids
        QList<QUuid> bhp_ids;
        for (auto var : model_->variables()->GetWellBHPVariables("PROD")) {
            bhp_ids.append(var->id());
        }

        // set all bhps to 5000 (which exceeds the max value defined, 3000)
        for (QUuid key : bhp_ids) {
            base_case_->set_real_variable_value(key, 5000);
        }
        EXPECT_FALSE(box_constraint_->CaseSatisfiesConstraint(base_case_));

        // set all bhps to 100 (which is less than the min value defined, 1000)
        for (QUuid key : bhp_ids) {
            base_case_->set_real_variable_value(key, 100);
        }
        EXPECT_FALSE(box_constraint_->CaseSatisfiesConstraint(base_case_));

        // set all bhps to 1125 (which is within the defined bounds)
        for (QUuid key : bhp_ids) {
            base_case_->set_real_variable_value(key, 1125);
        }
        EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));
    }

    TEST_F(BhpConstraintTest, SnappingValues) {
        // get the bhp variable ids
        QList<QUuid> bhp_ids;
        for (auto var : model_->variables()->GetWellBHPVariables("PROD")) {
            bhp_ids.append(var->id());
        }

        // set all bhps to 5000 (which exceeds the max value defined, 3000)
        for (QUuid key : bhp_ids) {
            base_case_->set_real_variable_value(key, 5000);
        }
        EXPECT_FALSE(box_constraint_->CaseSatisfiesConstraint(base_case_));
        // Snap the values back and recheck
        box_constraint_->SnapCaseToConstraints(base_case_);
        EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));

        // set all bhps to 100 (which is less than the min value defined, 1000)
        for (QUuid key : bhp_ids) {
            base_case_->set_real_variable_value(key, 100);
        }
        EXPECT_FALSE(box_constraint_->CaseSatisfiesConstraint(base_case_));
        // Snap the values back and recheck
        box_constraint_->SnapCaseToConstraints(base_case_);
        EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));
    }

}

