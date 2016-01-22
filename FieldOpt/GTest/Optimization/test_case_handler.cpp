/******************************************************************************
 *
 *
 *
 * Created: 01.12.2015 2015 by einar
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

#include "test_fixture_case.h"
#include "Optimization/case_handler.h"
#include <iostream>

namespace {

class CaseHandlerTest : public CaseTestFixture {
protected:
    CaseHandlerTest(){}
    virtual ~CaseHandlerTest(){}
    virtual void SetUp() {
        case_handler_ = new Optimization::CaseHandler();
        foreach (Optimization::Case *c, cases_) {
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
    EXPECT_STREQ(cases_[0]->id().toString().toLatin1().constData(), case_handler_->QueuedCases()[0]->id().toString().toLatin1().constData());
    EXPECT_STREQ(cases_[1]->id().toString().toLatin1().constData(), case_handler_->QueuedCases()[1]->id().toString().toLatin1().constData());
    EXPECT_STREQ(cases_[2]->id().toString().toLatin1().constData(), case_handler_->QueuedCases()[2]->id().toString().toLatin1().constData());
    EXPECT_STREQ(cases_[3]->id().toString().toLatin1().constData(), case_handler_->QueuedCases()[3]->id().toString().toLatin1().constData());
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
