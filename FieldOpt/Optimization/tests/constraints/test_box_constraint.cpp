#include "gtest/gtest.h"
#include "Optimization/constraints/box_constraint.h"
#include "Utilities/tests/test_resource_settings.h"
#include "Optimization/tests/test_resource_optimizer.h"

namespace {

    class BoxConstraintTest : public ::testing::Test, public TestResources::TestResourceOptimizer {
    protected:
        BoxConstraintTest() {
            box_constraint_ = new ::Optimization::Constraints::BoxConstraint(settings_optimizer_->constraints()->first(), model_->variables());
        }
        virtual ~BoxConstraintTest() {}
        virtual void SetUp() {}

        Optimization::Constraints::BoxConstraint *box_constraint_;
    };

    TEST_F(BoxConstraintTest, Constructor) {
        EXPECT_TRUE(true);
    }

    TEST_F(BoxConstraintTest, BaseCaseSatasfaction) {
        EXPECT_TRUE(box_constraint_->CaseSatisfiesConstraint(base_case_));
    }

    TEST_F(BoxConstraintTest, SatisfactionAfterModification) {
        // get the bhp variable ids
        QList<QUuid> bhp_ids;
        foreach (auto var, model_->variables()->GetWellBHPVariables("PROD")) {
            bhp_ids.append(var->id());
        }

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
        QList<QUuid> bhp_ids;
            foreach (auto var, model_->variables()->GetWellBHPVariables("PROD")) {
            bhp_ids.append(var->id());
        }

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

