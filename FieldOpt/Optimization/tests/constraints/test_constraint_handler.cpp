#include <gtest/gtest.h>
#include "Optimization/tests/test_resource_optimizer.h"
#include "Optimization/constraints/constraint_handler.h"
#include "Reservoir/tests/test_resource_grids.h"

namespace {

class ConstraintHandlerTest : public ::testing::Test, public TestResources::TestResourceOptimizer, public TestResources::TestResourceGrids {
protected:
    ConstraintHandlerTest() {
		// ESTABLISH MODEL OBJECT FOR TESTING
        if (settings_full_->verbosity()>2) std::cout << "... setting up constraint handler using test settings (test_constraint_handler.h)" << std::endl;
        constraint_handler_ = new ::Optimization::Constraints::ConstraintHandler(settings_optimizer_->constraints(),
                                                                                 model_->variables(), grid_5spot_);
    }
    virtual ~ConstraintHandlerTest() {}
    virtual void SetUp() {}

    Optimization::Constraints::ConstraintHandler *constraint_handler_;
};

TEST_F(ConstraintHandlerTest, Constructor) {
    if (settings_full_->verbosity()>2) std::cout << "Number of constraint types defined in driver.json: " << constraint_handler_->constraints().size() << std::endl;
    EXPECT_EQ(6, constraint_handler_->constraints().size());
}

TEST_F(ConstraintHandlerTest, BaseCaseSatisfyBHPConstraint) {
    if (settings_full_->verbosity()>2) std::cout << "Check if base case satisfies first constraint type (BHP)" << std::endl;
    EXPECT_TRUE(constraint_handler_->constraints().first()->CaseSatisfiesConstraint(base_case_));
}

TEST_F(ConstraintHandlerTest, BaseCaseSatisfyWellSplineLengthConstraint) {
    if (settings_full_->verbosity()>2) std::cout << "Check if base case breaks second constraint type (WellSplineLength)" << std::endl;
    EXPECT_FALSE(constraint_handler_->constraints()[1]->CaseSatisfiesConstraint(base_case_));
}
    
}
