/******************************************************************************
 *
 * test_properties.cpp
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

using namespace Model::Properties;

namespace {

class PropertiesTest : public ::testing::Test {
protected:
    PropertiesTest () { }
    virtual ~PropertiesTest () { }
};

TEST_F(PropertiesTest , BinaryPropertyConstructor) {
    BinaryProperty *binvar = new BinaryProperty(true);
    EXPECT_TRUE(binvar->value());
    EXPECT_FALSE(binvar->IsLocked());
    EXPECT_TRUE(binvar->type() == Property::Type::Binary);
}

TEST_F(PropertiesTest, DiscretePropertyConstructor) {
    DiscreteProperty *intvar = new DiscreteProperty(4);
    EXPECT_TRUE(intvar->value() == 4);
    EXPECT_FALSE(intvar->IsLocked());
    EXPECT_TRUE(intvar->type() == Property::Type::Discrete);
}

TEST_F(PropertiesTest, ContinousPropertyConstructor) {
    ContinousProperty *realvar = new ContinousProperty(2.0);
    EXPECT_TRUE(realvar->value() == 2.0);
    EXPECT_FALSE(realvar->IsLocked());
    EXPECT_TRUE(realvar->type() == Property::Type::Continous);
}

TEST_F(PropertiesTest, Locking) {
    BinaryProperty *binvar = new BinaryProperty(true);
    DiscreteProperty *intvar = new DiscreteProperty(4);
    ContinousProperty *realvar = new ContinousProperty(2.0);

    // Lock and check
    binvar->Lock(); intvar->Lock(); realvar->Lock();
    EXPECT_TRUE(binvar->IsLocked());
    EXPECT_TRUE(intvar->IsLocked());
    EXPECT_TRUE(realvar->IsLocked());

    // Attempt modifications;
    EXPECT_THROW(binvar->setValue(false), PropertyLockedException);
    EXPECT_THROW(intvar->Add(1), PropertyLockedException);
    EXPECT_THROW(intvar->setValue(1), PropertyLockedException);
    EXPECT_THROW(realvar->Add(2.0), PropertyLockedException);
    EXPECT_THROW(realvar->setValue(0.0), PropertyLockedException);

    // Verify values not changed
    EXPECT_TRUE(binvar->value());
    EXPECT_TRUE(intvar->value() == 4);
    EXPECT_TRUE(realvar->value() == 2.0);
}

TEST_F(PropertiesTest, DiscretePropertyAddition) {
    DiscreteProperty *intvar = new DiscreteProperty(5);
    intvar->Add(2);
    EXPECT_TRUE(intvar->value() == 7);
}

TEST_F(PropertiesTest, ContinousPropertyAddition) {
    ContinousProperty *realvar = new ContinousProperty(7.0);
    realvar->Add(-2.5);
    EXPECT_TRUE(realvar->value() == 4.5);
}

TEST_F(PropertiesTest, DiscretePropertyEquals) {
    DiscreteProperty *intvar_1 = new DiscreteProperty(1);
    DiscreteProperty *intvar_2 = new DiscreteProperty(1);
    DiscreteProperty *intvar_3 = new DiscreteProperty(2);
    EXPECT_TRUE(intvar_1->Equals(intvar_2));
    EXPECT_FALSE(intvar_1->Equals(intvar_3));
}

TEST_F(PropertiesTest, ContinousPropertyEquals) {
    ContinousProperty *realvar_1 = new ContinousProperty(2.0);
    ContinousProperty *realvar_2 = new ContinousProperty(2.0);
    ContinousProperty *realvar_3 = new ContinousProperty(3.0);
    ContinousProperty *realvar_4 = new ContinousProperty(2.1);
    EXPECT_TRUE(realvar_1->Equals(realvar_2));
    EXPECT_FALSE(realvar_1->Equals(realvar_3));
    EXPECT_FALSE(realvar_1->Equals(realvar_4, 0.09)); // Should be outside epsilon
    EXPECT_TRUE(realvar_1->Equals(realvar_4, 0.11)); // Should be inside epsilon
}

}
