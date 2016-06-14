#include <gtest/gtest.h>
#include <constraints/rate_constraint.h>
#include "test_resource_optimizer.h"

namespace {
    class RateConstraintTest : public ::testing::Test, public TestResources::TestResourceOptimizer {

    public:
        RateConstraintTest() : Test() {
            auto *rate_var = new Model::Properties::ContinousProperty(1200.0);
            rate_var->setName("Rate#INJ#0");
            model_->variables()->AddVariable(rate_var);
            constraint_ = new Optimization::Constraints::RateConstraint(settings_optimizer_->constraints()[2], model_->variables());
            c = new Optimization::Case(model_->variables()->GetBinaryVariableValues(),
                                       model_->variables()->GetDiscreteVariableValues(),
                                       model_->variables()->GetContinousVariableValues());
        }
        Optimization::Constraints::RateConstraint *constraint_;
        Optimization::Case *c;
    };


    TEST_F(RateConstraintTest, Constructor) {
        EXPECT_TRUE(true);
    }

    TEST_F(RateConstraintTest, Initial) {
        EXPECT_TRUE(constraint_->CaseSatisfiesConstraint(c));
    }

    TEST_F(RateConstraintTest, AfterModification) {
        EXPECT_TRUE(constraint_->CaseSatisfiesConstraint(c));
        auto rate_vars = model_->variables()->GetWellRateVariables("INJ");
        for (auto var : rate_vars) {
            c->set_real_variable_value(var->id(), 900);
        }
        EXPECT_FALSE(constraint_->CaseSatisfiesConstraint(c));
        for (auto var : rate_vars) {
            c->set_real_variable_value(var->id(), 1300);
        }
        EXPECT_TRUE(constraint_->CaseSatisfiesConstraint(c));
        for (auto var : rate_vars) {
            c->set_real_variable_value(var->id(), 1600);
        }
        EXPECT_FALSE(constraint_->CaseSatisfiesConstraint(c));
    }


    TEST_F(RateConstraintTest, Snapping) {
        EXPECT_TRUE(constraint_->CaseSatisfiesConstraint(c));
        auto rate_vars = model_->variables()->GetWellRateVariables("INJ");

        // Testing snap to minimum
        for (auto var : rate_vars) {
            c->set_real_variable_value(var->id(), 900);
        }
        EXPECT_FALSE(constraint_->CaseSatisfiesConstraint(c));
        constraint_->SnapCaseToConstraints(c);
        EXPECT_TRUE(constraint_->CaseSatisfiesConstraint(c));
        for (auto var : rate_vars) {
            EXPECT_FLOAT_EQ(1200, c->real_variables()[var->id()]);
        }

        // Testing snap to maximum
        for (auto var : rate_vars) {
            c->set_real_variable_value(var->id(), 1800);
        }
        EXPECT_FALSE(constraint_->CaseSatisfiesConstraint(c));
        constraint_->SnapCaseToConstraints(c);
        EXPECT_TRUE(constraint_->CaseSatisfiesConstraint(c));
        for (auto var : rate_vars) {
            EXPECT_FLOAT_EQ(1400, c->real_variables()[var->id()]);
        }
    }
}
