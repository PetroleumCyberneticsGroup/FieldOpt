#include "GTest/Optimization/test_fixture_case.h"
#include "../Runner/bookkeeper.h"
#include "Optimization/optimizers/compass_search.h"

namespace {

class BookkeeperTest : public CaseTestFixture {
protected:
    BookkeeperTest() {
        compass_search_ = new ::Optimization::Optimizers::CompassSearch(optimizer_settings_, base_case_, model_->variables());
        bookkeeper_ = new Runner::Bookkeeper(settings_, compass_search_->case_handler());
        c1 = compass_search_->GetCaseForEvaluation();
        c2 = compass_search_->GetCaseForEvaluation();
        c3 = compass_search_->GetCaseForEvaluation();
    }
    virtual ~BookkeeperTest() {}
    virtual void SetUp() {}

    Runner::Bookkeeper *bookkeeper_;
    Optimization::Optimizer *compass_search_;
    Optimization::Case *c1;
    Optimization::Case *c2;
    Optimization::Case *c3;
};

TEST_F(BookkeeperTest, Bookkeeping) {
    EXPECT_FALSE(base_case_->Equals(c1));
    EXPECT_FALSE(base_case_->Equals(c2));
    EXPECT_FALSE(base_case_->Equals(c3));
    EXPECT_TRUE(base_case_->Equals(base_case_));
    EXPECT_TRUE(bookkeeper_->IsEvaluated(base_case_));
    EXPECT_FALSE(bookkeeper_->IsEvaluated(c1));
    EXPECT_FALSE(bookkeeper_->IsEvaluated(c2));
    EXPECT_FALSE(bookkeeper_->IsEvaluated(c3));

    compass_search_->SubmitEvaluatedCase(c1);
    EXPECT_TRUE(bookkeeper_->IsEvaluated(c1));
}

}
