/******************************************************************************
 *
 *
 *
 * Created: 07.12.2015 2015 by einar
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

#include "GTest/Optimization/test_fixture_case.h"
#include "Optimization/optimizers/compass_search.h"

namespace {

class CompassSearchTest : public CaseTestFixture {
protected:
    CompassSearchTest() {
        compass_search_ = new ::Optimization::Optimizers::CompassSearch(optimizer_settings_, base_case_, model_->variables());
    }
    virtual ~CompassSearchTest() {}
    virtual void SetUp() {}

    Optimization::Optimizer *compass_search_;
};

TEST_F(CompassSearchTest, Constructor) {
    EXPECT_FLOAT_EQ(1000.0, compass_search_->GetTentativeBestCase()->objective_function_value());
}

TEST_F(CompassSearchTest, GetNewCases) {
    Optimization::Case *new_case_1 = compass_search_->GetCaseForEvaluation();
    Optimization::Case *new_case_2 = compass_search_->GetCaseForEvaluation();
    Optimization::Case *new_case_3 = compass_search_->GetCaseForEvaluation();
    Optimization::Case *new_case_4 = compass_search_->GetCaseForEvaluation();
    EXPECT_FALSE(new_case_1->id() == new_case_2->id());
    EXPECT_FALSE(new_case_3->id() == new_case_4->id());
}

TEST_F(CompassSearchTest, Pseudoiterations) {
    Optimization::Case *tentative_best_0 = compass_search_->GetTentativeBestCase();
    for (int i = 0; i < 50; ++i) {
        Optimization::Case *new_case = compass_search_->GetCaseForEvaluation();
        new_case->set_objective_function_value((i%3)*700);
        compass_search_->SubmitEvaluatedCase(new_case);
    }
    Optimization::Case *tentative_best_1 = compass_search_->GetTentativeBestCase();
    EXPECT_TRUE(tentative_best_1->objective_function_value() > tentative_best_0->objective_function_value());

    for (int i = 51; i < 100; ++i) {
        Optimization::Case *new_case = compass_search_->GetCaseForEvaluation();
        new_case->set_objective_function_value((i%3)*800);
        compass_search_->SubmitEvaluatedCase(new_case);
    }
    Optimization::Case *tentative_best_2 = compass_search_->GetTentativeBestCase();
    EXPECT_TRUE(tentative_best_2->objective_function_value() > tentative_best_1->objective_function_value());
}

}
