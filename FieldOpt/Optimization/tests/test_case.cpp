/******************************************************************************
 *
 *
 *
 * Created: 30.11.2015 2015 by einar
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

namespace {


class CaseTest : public CaseTestFixture {
protected:
    CaseTest(){}
    virtual ~CaseTest(){}
};

TEST_F(CaseTest, Constructor) {
    EXPECT_NO_THROW();
}

TEST_F(CaseTest, UUIDs) {
    EXPECT_FALSE(cases_[0]->id().isNull());
    EXPECT_FALSE(cases_[1]->id().isNull());
    EXPECT_FALSE(cases_[2]->id().isNull());
    EXPECT_FALSE(cases_[3]->id().isNull());
}

TEST_F(CaseTest, ObjectiveValues) {
    EXPECT_THROW(cases_[0]->objective_function_value(), Optimization::ObjectiveFunctionException);
    EXPECT_FLOAT_EQ(100.0, cases_[1]->objective_function_value());
    EXPECT_FLOAT_EQ(-50.0, cases_[2]->objective_function_value());
    EXPECT_FLOAT_EQ(-50.0, cases_[3]->objective_function_value());
}

TEST_F(CaseTest, Equals) {
    EXPECT_FALSE(cases_[0]->Equals(cases_[1]));
    EXPECT_TRUE(cases_[2]->Equals(cases_[3]));
}

TEST_F(CaseTest, NumberOfVariables) {
    EXPECT_EQ(0, cases_[0]->binary_variables().size());
    EXPECT_EQ(3, cases_[0]->integer_variables().size());
    EXPECT_EQ(0, cases_[0]->real_variables().size());

    EXPECT_EQ(0, cases_[1]->binary_variables().size());
    EXPECT_EQ(0, cases_[1]->integer_variables().size());
    EXPECT_EQ(3, cases_[1]->real_variables().size());

    EXPECT_EQ(4, cases_[2]->binary_variables().size());
    EXPECT_EQ(3, cases_[2]->integer_variables().size());
    EXPECT_EQ(3, cases_[2]->real_variables().size());

    EXPECT_EQ(4, cases_[3]->binary_variables().size());
    EXPECT_EQ(3, cases_[3]->integer_variables().size());
    EXPECT_EQ(3, cases_[3]->real_variables().size());
}

TEST_F(CaseTest, VariableValues) {
    foreach (int value, integer_variables_.values()) {
        EXPECT_TRUE(value == 1 || value == 2 || value == 5);
    }

    foreach (double value, real_variables_.values()) {
        EXPECT_TRUE(value == 1.0 || value == 4.0 || value == 2.5);
    }
}

TEST_F(CaseTest, CopyConstructor) {
    Optimization::Case *copy = new Optimization::Case(cases_[0]);
    EXPECT_FALSE(copy->id() == cases_[0]->id());
    EXPECT_TRUE(copy->Equals(cases_[0]));
}

TEST_F(CaseTest, PerturbMethod) {
    // Perturb the first case's first integer variable in positive direction
    QList<Optimization::Case *> perturbations_1 = cases_[0]->Perturb(cases_[0]->integer_variables().keys().first(),
            Optimization::Case::SIGN::PLUS, 7);
    EXPECT_EQ(1, perturbations_1.size());
    EXPECT_FALSE(perturbations_1.first()->id() == cases_[0]->id());
    EXPECT_FALSE(perturbations_1.first()->Equals(cases_[0]));
    EXPECT_TRUE(cases_[0]->integer_variables().values().first() + 7 == perturbations_1.first()->integer_variables().values().first());

    // Perturb the second case's first real variable in both directions
    QList<Optimization::Case *> perturbations_2 = cases_[1]->Perturb(cases_[1]->real_variables().keys().first(),
            Optimization::Case::SIGN::PLUSMINUS, 5);
    EXPECT_EQ(2, perturbations_2.size());
    EXPECT_FALSE(perturbations_2[0]->id() == cases_[1]->id());
    EXPECT_FALSE(perturbations_2[1]->id() == cases_[1]->id());
    EXPECT_FALSE(perturbations_2[0]->Equals(cases_[1]));
    EXPECT_FALSE(perturbations_2[1]->Equals(cases_[1]));
    EXPECT_FALSE(perturbations_2[0]->Equals(perturbations_2[1]));
    EXPECT_TRUE(cases_[1]->real_variables().values().first() + 5 == perturbations_2[0]->real_variables().values().first());
    EXPECT_TRUE(cases_[1]->real_variables().values().first() - 5 == perturbations_2[1]->real_variables().values().first());
}

}

