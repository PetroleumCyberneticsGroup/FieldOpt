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
#include "Model/properties/property.h"
#include "Model/properties/binary_property.h"
#include "Model/properties/discrete_property.h"
#include "Model/properties/continous_property.h"
#include "Model/properties/variable_property_container.h"

using namespace Model::Properties;

namespace {

class VariableContainerTest : public ::testing::Test {
protected:
    VariableContainerTest () {
        variable_container_ = new VariablePropertyContainer();
    }
    virtual ~VariableContainerTest () { }

    virtual void SetUp() {
        variable_container_->AddVariable(new ContinousProperty(2.0));
        variable_container_->AddVariable(new ContinousProperty(5.0));
        variable_container_->AddVariable(new ContinousProperty(1.0));
        variable_container_->AddVariable(new DiscreteProperty(5));
        variable_container_->AddVariable(new BinaryProperty(false));
        variable_container_->AddVariable(new BinaryProperty(true));
    }

    VariablePropertyContainer *variable_container_;
};

TEST_F(VariableContainerTest, Constructor) {
    // If the lists exist, the constructor has been run
    EXPECT_EQ(variable_container_->BinaryVariableSize(), 2);
    EXPECT_EQ(variable_container_->DiscreteVariableSize(), 1);
    EXPECT_EQ(variable_container_->ContinousVariableSize(), 3);
}

TEST_F(VariableContainerTest, Retrieval) {
    EXPECT_EQ(variable_container_->GetBinaryVariable(0)->value(), false);
    EXPECT_EQ(variable_container_->GetDiscreteVariable(0)->value(), 5);
    EXPECT_FLOAT_EQ(variable_container_->GetContinousVariable(2)->value(), 1.0);
    EXPECT_EQ(variable_container_->GetBinaryVariableValues()[0], false);
    EXPECT_EQ(variable_container_->GetDiscreteVariableValues()[0], 5);
    EXPECT_FLOAT_EQ(variable_container_->GetContinousVariableValues()[2], 1.0);
    EXPECT_THROW(variable_container_->GetContinousVariable(4), VariableIdDoesNotExistException);
    EXPECT_THROW(variable_container_->GetBinaryVariable(-1), VariableIdDoesNotExistException);
    EXPECT_THROW(variable_container_->GetDiscreteVariable(10), VariableIdDoesNotExistException);
}

TEST_F(VariableContainerTest, Deletion) {
    EXPECT_NO_THROW(variable_container_->DeleteBinaryVariable(0));
    EXPECT_THROW(variable_container_->GetBinaryVariable(0), VariableIdDoesNotExistException);
    EXPECT_THROW(variable_container_->DeleteDiscreteVariable(1), VariableIdDoesNotExistException);
    EXPECT_NO_THROW(variable_container_->DeleteDiscreteVariable(0));
    EXPECT_EQ(variable_container_->DiscreteVariableSize(), 0);
    EXPECT_NO_THROW(variable_container_->DeleteContinousVariable(1));
    EXPECT_THROW(variable_container_->GetContinousVariable(1), VariableIdDoesNotExistException);
    EXPECT_DOUBLE_EQ(variable_container_->GetContinousVariable(2)->value(), 1.0);
}

}
