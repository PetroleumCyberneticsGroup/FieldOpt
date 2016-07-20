#include "gtest/gtest.h"
#include "Optimization/constraints/bhp_constraint.h"
#include "Utilities/tests/test_resource_settings.h"
#include "Optimization/tests/test_resource_optimizer.h"

namespace {

    class BhpConstraintTest : public ::testing::Test, public TestResources::TestResourceOptimizer {
    protected:
        BhpConstraintTest() {
            // USE FIRST DEFINED CONSTRAINT FOR BHP TESTING
            if (settings_full_->verbosity()>2){
                std::cout << "... using first constraint defined in driver.json (BHP); well: " << settings_optimizer_->constraints()[0].well.toStdString() << std::endl;
            }

            box_constraint_ = new ::Optimization::Constraints::BhpConstraint(
                settings_optimizer_->constraints()[0], model_->variables());
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

