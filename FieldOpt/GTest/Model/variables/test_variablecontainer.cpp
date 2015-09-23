/******************************************************************************
 *
 * test_variablecontainer.cpp
 *
 * Created: 23.09.2015 2015 by einar
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

#include <gtest/gtest.h>
#include "Model/reservoir/grid/xyzcoordinate.h"
#include "Model/variables/variable.h"
#include "Model/variables/binaryvariable.h"
#include "Model/variables/integervariable.h"
#include "Model/variables/realvariable.h"
#include "Model/variables/variablecontainer.h"

using namespace Model::Variables;

namespace {

class VariableContainerTest : public ::testing::Test {
protected:
    VariableContainerTest () {
        variable_container_ = new VariableContainer();
    }
    virtual ~VariableContainerTest () { }

    virtual void SetUp() {
        variable_container_->AddVariable(new RealVariable(2.0));
        variable_container_->AddVariable(new RealVariable(5.0));
        variable_container_->AddVariable(new RealVariable(1.0));
        variable_container_->AddVariable(new IntegerVariable(5));
        variable_container_->AddVariable(new BinaryVariable(false));
        variable_container_->AddVariable(new BinaryVariable(true));
    }

    VariableContainer *variable_container_;
};

TEST_F(VariableContainerTest, Constructor) {
    // If the lists exist, the constructor has been run
    EXPECT_GT(variable_container_->binaryVariables()->size(), 0);
}

TEST_F(VariableContainerTest, ListLengths) {
    EXPECT_EQ(variable_container_->binaryVariables()->size(), 2);
    EXPECT_EQ(variable_container_->integerVariables()->size(), 1);
    EXPECT_EQ(variable_container_->realVariables()->size(), 3);
}

TEST_F(VariableContainerTest, Retrieval) {
    EXPECT_EQ(variable_container_->binaryVariables()->first()->value(), false);
    EXPECT_EQ(variable_container_->integerVariables()->at(0)->value(), 5);
    EXPECT_EQ(variable_container_->realVariables()->last()->value(), 1.0);
}

}
