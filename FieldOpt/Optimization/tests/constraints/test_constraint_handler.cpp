#include <gtest/gtest.h>
#include "Optimization/tests/test_resource_optimizer.h"
#include "Optimization/constraints/constraint_handler.h"

namespace {

class ConstraintHandlerTest : public ::testing::Test, public TestResources::TestResourceOptimizer {
protected:
    ConstraintHandlerTest() {
        constraint_handler_ = new ::Optimization::Constraints::ConstraintHandler(settings_optimizer_->constraints(), model_->variables());
    }
    virtual ~ConstraintHandlerTest() {}
    virtual void SetUp() {}

    Optimization::Constraints::ConstraintHandler *constraint_handler_;
};

TEST_F(ConstraintHandlerTest, Constructor) {
    EXPECT_EQ(1, constraint_handler_->constraints().size());
    EXPECT_STREQ(constraint_handler_->constraints().first()->name().toLatin1().constData(), "PROD-BHP-1");
}

TEST_F(ConstraintHandlerTest, BaseCaseSatasfaction) {
    EXPECT_TRUE(constraint_handler_->constraints().first()->CaseSatisfiesConstraint(base_case_));
}

}
