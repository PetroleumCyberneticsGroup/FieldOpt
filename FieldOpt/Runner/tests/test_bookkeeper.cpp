/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <gtest/gtest.h>
#include "Optimization/tests/test_resource_optimizer.h"
#include "../Runner/bookkeeper.h"
#include "Optimization/optimizers/compass_search.h"
#include "Settings/tests/test_resource_example_file_paths.hpp"
#include "test_resource_runner.hpp"

namespace {

class BookkeeperTest : public ::testing::Test,
                       public TestResources::TestResourceOptimizer
{
 protected:
  BookkeeperTest() {
      test_case_2r_->set_objective_function_value(100.0);
      compass_search_ = new ::Optimization::Optimizers::CompassSearch(settings_compass_search_max_unconstr_,
                                                                      test_case_2r_,
                                                                      model_->variables(),
                                                                      model_->grid(),
                                                                      logger_
      );
      bookkeeper_ = new Runner::Bookkeeper(settings_full_, compass_search_->case_handler());
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
    EXPECT_TRUE(test_case_2r_->Equals(test_case_2r_));
    EXPECT_TRUE(bookkeeper_->IsEvaluated(test_case_2r_));
    EXPECT_FALSE(bookkeeper_->IsEvaluated(c1));
    EXPECT_FALSE(bookkeeper_->IsEvaluated(c2));
    EXPECT_FALSE(bookkeeper_->IsEvaluated(c3));

    c1->set_objective_function_value(100);
    compass_search_->SubmitEvaluatedCase(c1);
    EXPECT_TRUE(bookkeeper_->IsEvaluated(c1));
}

}
