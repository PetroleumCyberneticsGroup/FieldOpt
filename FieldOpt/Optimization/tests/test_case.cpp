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

#include <gtest/gtest.h>
#include "test_resource_cases.h"
#include <QList>

namespace {


    class CaseTest : public ::testing::Test, public TestResourceCases {

    public:

        CaseTest() : Test() { }

        virtual ~CaseTest() { }

    protected:
        virtual void TearDown() {

        }

        virtual void SetUp() {

        }


    };

    TEST_F(CaseTest, Constructor) {
        EXPECT_NO_THROW();
    }

    TEST_F(CaseTest, UUIDs) {
        EXPECT_FALSE(test_case_1_->id().isNull());
        EXPECT_FALSE(test_case_2_->id().isNull());
        EXPECT_FALSE(test_case_3_->id().isNull());
        EXPECT_FALSE(test_case_4_->id().isNull());
    }

    TEST_F(CaseTest, ObjectiveValues) {
        EXPECT_THROW(test_case_1_->objective_function_value(), Optimization::ObjectiveFunctionException);
        EXPECT_FLOAT_EQ(100.0, test_case_2_->objective_function_value());
        EXPECT_FLOAT_EQ(-50.0, test_case_3_->objective_function_value());
        EXPECT_FLOAT_EQ(-50.0, test_case_4_->objective_function_value());
    }

    TEST_F(CaseTest, Equals) {
        EXPECT_FALSE(test_case_1_->Equals(test_case_2_));
        EXPECT_TRUE(test_case_3_->Equals(test_case_4_));
    }

    TEST_F(CaseTest, NumberOfVariables) {
        EXPECT_EQ(0, test_case_1_->binary_variables().size());
        EXPECT_EQ(3, test_case_1_->integer_variables().size());
        EXPECT_EQ(0, test_case_1_->real_variables().size());

        EXPECT_EQ(0, test_case_2_->binary_variables().size());
        EXPECT_EQ(0, test_case_2_->integer_variables().size());
        EXPECT_EQ(3, test_case_2_->real_variables().size());

        EXPECT_EQ(4, test_case_3_->binary_variables().size());
        EXPECT_EQ(3, test_case_3_->integer_variables().size());
        EXPECT_EQ(3, test_case_3_->real_variables().size());

        EXPECT_EQ(4, test_case_4_->binary_variables().size());
        EXPECT_EQ(3, test_case_4_->integer_variables().size());
        EXPECT_EQ(3, test_case_4_->real_variables().size());
    }

    TEST_F(CaseTest, CopyConstructor) {
        Optimization::Case *copy = new Optimization::Case(test_case_1_);
        EXPECT_FALSE(copy->id() == test_case_1_->id());
        EXPECT_TRUE(copy->Equals(test_case_1_));
    }

    TEST_F(CaseTest, PerturbMethod) {
        // Perturb the first case's first integer variable in positive direction
        QList<Optimization::Case *> perturbations_1 = test_case_1_->Perturb(test_case_1_->integer_variables().keys().first(),
                                                                            Optimization::Case::SIGN::PLUS, 7);
        EXPECT_EQ(1, perturbations_1.size());
        EXPECT_FALSE(perturbations_1.first()->id() == test_case_1_->id());
        EXPECT_FALSE(perturbations_1.first()->Equals(test_case_1_));
        EXPECT_TRUE(test_case_1_->integer_variables().values().first() + 7 == perturbations_1.first()->integer_variables().values().first());

        // Perturb the second case's first real variable in both directions
        QList<Optimization::Case *> perturbations_2 = test_case_2_->Perturb(test_case_2_->real_variables().keys().first(),
                                                                            Optimization::Case::SIGN::PLUSMINUS, 5);
        EXPECT_EQ(2, perturbations_2.size());
        EXPECT_FALSE(perturbations_2[0]->id() == test_case_2_->id());
        EXPECT_FALSE(perturbations_2[1]->id() == test_case_2_->id());
        EXPECT_FALSE(perturbations_2[0]->Equals(test_case_2_));
        EXPECT_FALSE(perturbations_2[1]->Equals(test_case_2_));
        EXPECT_FALSE(perturbations_2[0]->Equals(perturbations_2[1]));
        EXPECT_TRUE(test_case_2_->real_variables().values().first() + 5 == perturbations_2[0]->real_variables().values().first());
        EXPECT_TRUE(test_case_2_->real_variables().values().first() - 5 == perturbations_2[1]->real_variables().values().first());
    }

}
