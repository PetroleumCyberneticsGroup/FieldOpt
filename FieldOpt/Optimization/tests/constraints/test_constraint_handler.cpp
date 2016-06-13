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
    EXPECT_EQ(5, constraint_handler_->constraints().size());
}

TEST_F(ConstraintHandlerTest, BaseCaseSatasfaction) {
    EXPECT_TRUE(constraint_handler_->constraints().first()->CaseSatisfiesConstraint(base_case_));
}

}
