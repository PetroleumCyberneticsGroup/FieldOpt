/******************************************************************************
 *
 * test_variables.cpp
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

using namespace Model::Variables;

namespace {

class VariablesTest : public ::testing::Test {
protected:
    VariablesTest () { }
    virtual ~VariablesTest () { }
};

TEST_F(VariablesTest , BinaryVariableConstructor) {
    BinaryVariable *binvar = new BinaryVariable(true);
    EXPECT_TRUE(binvar->value());
    EXPECT_FALSE(binvar->IsLocked());
    EXPECT_TRUE(binvar->type() == Variable::Type::Binary);
}

TEST_F(VariablesTest, IntegerVariableConstructor) {
    IntegerVariable *intvar = new IntegerVariable(4);
    EXPECT_TRUE(intvar->value() == 4);
    EXPECT_FALSE(intvar->IsLocked());
    EXPECT_TRUE(intvar->type() == Variable::Type::Integer);
}

TEST_F(VariablesTest, RealVariableConstructor) {
    RealVariable *realvar = new RealVariable(2.0);
    EXPECT_TRUE(realvar->value() == 2.0);
    EXPECT_FALSE(realvar->IsLocked());
    EXPECT_TRUE(realvar->type() == Variable::Type::Real);
}

TEST_F(VariablesTest, Locking) {
    BinaryVariable *binvar = new BinaryVariable(true);
    IntegerVariable *intvar = new IntegerVariable(4);
    RealVariable *realvar = new RealVariable(2.0);

    // Lock and check
    binvar->Lock(); intvar->Lock(); realvar->Lock();
    EXPECT_TRUE(binvar->IsLocked());
    EXPECT_TRUE(intvar->IsLocked());
    EXPECT_TRUE(realvar->IsLocked());

    // Attempt modifications;
    EXPECT_THROW(binvar->setValue(false), VariableLockedException);
    EXPECT_THROW(intvar->Add(1), VariableLockedException);
    EXPECT_THROW(intvar->setValue(1), VariableLockedException);
    EXPECT_THROW(realvar->Add(2.0), VariableLockedException);
    EXPECT_THROW(realvar->setValue(0.0), VariableLockedException);

    // Verify values not changed
    EXPECT_TRUE(binvar->value());
    EXPECT_TRUE(intvar->value() == 4);
    EXPECT_TRUE(realvar->value() == 2.0);
}

TEST_F(VariablesTest, IntegerVariableAddition) {
    IntegerVariable *intvar = new IntegerVariable(5);
    intvar->Add(2);
    EXPECT_TRUE(intvar->value() == 7);
}

TEST_F(VariablesTest, RealVariableAddition) {
    RealVariable *realvar = new RealVariable(7.0);
    realvar->Add(-2.5);
    EXPECT_TRUE(realvar->value() == 4.5);
}

TEST_F(VariablesTest, IntegerVariableEquals) {
    IntegerVariable *intvar_1 = new IntegerVariable(1);
    IntegerVariable *intvar_2 = new IntegerVariable(1);
    IntegerVariable *intvar_3 = new IntegerVariable(2);
    EXPECT_TRUE(intvar_1->Equals(intvar_2));
    EXPECT_FALSE(intvar_1->Equals(intvar_3));
}

TEST_F(VariablesTest, RealVariableEquals) {
    RealVariable *realvar_1 = new RealVariable(2.0);
    RealVariable *realvar_2 = new RealVariable(2.0);
    RealVariable *realvar_3 = new RealVariable(3.0);
    RealVariable *realvar_4 = new RealVariable(2.1);
    EXPECT_TRUE(realvar_1->Equals(realvar_2));
    EXPECT_FALSE(realvar_1->Equals(realvar_3));
    EXPECT_FALSE(realvar_1->Equals(realvar_4, 0.09)); // Should be outside epsilon
    EXPECT_TRUE(realvar_1->Equals(realvar_4, 0.11)); // Should be inside epsilon
}

}
