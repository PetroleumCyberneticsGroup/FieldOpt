#include <gtest/gtest.h>
#include "Optimization/case_handler.h"
#include "Optimization/tests/test_resource_cases.h"

namespace {

class CaseHandlerTest : public ::testing::Test, public TestResources::TestResourceCases {
protected:
    CaseHandlerTest(){}
    virtual ~CaseHandlerTest(){}
    virtual void SetUp() {
        case_handler_ = new Optimization::CaseHandler();
        foreach (Optimization::Case *c, trivial_cases_) {
            case_handler_->AddNewCase(c);
        }
    }
    virtual void TearDown() {
        delete case_handler_;
    }

    Optimization::CaseHandler *case_handler_;
};

TEST_F(CaseHandlerTest, Constructor) {
    EXPECT_NO_THROW();
}

TEST_F(CaseHandlerTest, InitialQueue) {
    EXPECT_EQ(4, case_handler_->QueuedCases().size());
    EXPECT_EQ(0, case_handler_->CasesBeingEvaluated().size());
    EXPECT_EQ(0, case_handler_->EvaluatedCases().size());
    EXPECT_EQ(0, case_handler_->RecentlyEvaluatedCases().size());
}

TEST_F(CaseHandlerTest, CaseIDs) {
    EXPECT_STREQ(trivial_cases_[0]->id().toString().toLatin1().constData(), case_handler_->QueuedCases()[0]->id().toString().toLatin1().constData());
    EXPECT_STREQ(trivial_cases_[1]->id().toString().toLatin1().constData(), case_handler_->QueuedCases()[1]->id().toString().toLatin1().constData());
    EXPECT_STREQ(trivial_cases_[2]->id().toString().toLatin1().constData(), case_handler_->QueuedCases()[2]->id().toString().toLatin1().constData());
    EXPECT_STREQ(trivial_cases_[3]->id().toString().toLatin1().constData(), case_handler_->QueuedCases()[3]->id().toString().toLatin1().constData());
}

TEST_F(CaseHandlerTest, GetNewCase) {
    case_handler_->GetNextCaseForEvaluation();
    EXPECT_EQ(3, case_handler_->QueuedCases().size());
    EXPECT_EQ(1, case_handler_->CasesBeingEvaluated().size());
    EXPECT_EQ(0, case_handler_->EvaluatedCases().size());
    EXPECT_EQ(0, case_handler_->RecentlyEvaluatedCases().size());
}

TEST_F(CaseHandlerTest, SetCaseEvaluated) {
    Optimization::Case *next_case = case_handler_->GetNextCaseForEvaluation();
    next_case->set_objective_function_value(123.0);
    case_handler_->SetCaseEvaluated(next_case->id());
    EXPECT_EQ(3, case_handler_->QueuedCases().size());
    EXPECT_EQ(0, case_handler_->CasesBeingEvaluated().size());
    EXPECT_EQ(1, case_handler_->EvaluatedCases().size());
    EXPECT_EQ(1, case_handler_->RecentlyEvaluatedCases().size());
    EXPECT_FLOAT_EQ(123.0, case_handler_->RecentlyEvaluatedCases().first()->objective_function_value());
}

TEST_F(CaseHandlerTest, ClearRecentlyEvaluated) {
    Optimization::Case *next_case = case_handler_->GetNextCaseForEvaluation();
    next_case->set_objective_function_value(123.0);
    case_handler_->SetCaseEvaluated(next_case->id());
    EXPECT_EQ(3, case_handler_->QueuedCases().size());
    EXPECT_EQ(0, case_handler_->CasesBeingEvaluated().size());
    EXPECT_EQ(1, case_handler_->EvaluatedCases().size());
    EXPECT_EQ(1, case_handler_->RecentlyEvaluatedCases().size());

    case_handler_->ClearRecentlyEvaluatedCases();
    EXPECT_EQ(3, case_handler_->QueuedCases().size());
    EXPECT_EQ(0, case_handler_->CasesBeingEvaluated().size());
    EXPECT_EQ(1, case_handler_->EvaluatedCases().size());
    EXPECT_EQ(0, case_handler_->RecentlyEvaluatedCases().size());
    EXPECT_FLOAT_EQ(123.0, case_handler_->EvaluatedCases().first()->objective_function_value());
}

// clear recent



}
