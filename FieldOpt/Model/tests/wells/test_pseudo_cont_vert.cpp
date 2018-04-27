/******************************************************************************
   Created by einar on 8/16/17.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "Reservoir/tests/test_resource_grids.h"
#include "Model/tests/test_resource_model_setting_snippets.hpp"
#include "Model/wells/wellbore/pseudo_cont_vert.h"

namespace {

class PseudoContVertTest : public ::testing::Test, public TestResources::TestResourceGrids {

 protected:
  PseudoContVertTest() {
  }
  virtual ~PseudoContVertTest() {}
  virtual void SetUp() {}
};

TEST_F(PseudoContVertTest, ConstructorAndGetter) {
    auto settings = Settings::Model(TestResources::TestResourceModelSettingSnippets::model_pseudocont_2_prods_var_pos_and_bhp(), "");
    auto wsettings = settings.wells()[0];
    auto varcont = new Model::Properties::VariablePropertyContainer();
    auto well = Model::Wells::Wellbore::PseudoContVert(wsettings, varcont, TestResources::TestResourceGrids::grid_5spot_);

    auto block = well.GetWellBlock();

    // These should be 1 higher, as well blocks use 1-indexing instead of 0-indexing.
    EXPECT_EQ(block->i(), 6);
    EXPECT_EQ(block->j(), 6);
    EXPECT_EQ(block->k(), 1);
}

}
