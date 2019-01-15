/******************************************************************************
   Created by einar on 2/27/17.
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
#include <QtCore/QDate>
#include <iostream>
#include "Utilities/math.hpp"
#include <vector>

using namespace std;

namespace {

class MathTest : public testing::Test {

};


TEST_F(MathTest, Average) {
    vector<int> l1 = {1, 1, 1, 1};
    vector<int> l2 = {2, 2};
    vector<int> l3 = {0};
    vector<int> l4 = {1, 2, 1, 2, 1, 2};
    EXPECT_FLOAT_EQ(1, calc_average(l1));
    EXPECT_FLOAT_EQ(2, calc_average(l2));
    EXPECT_FLOAT_EQ(0, calc_average(l3));
    EXPECT_FLOAT_EQ(1.5, calc_average(l4));
}

TEST_F(MathTest, Median) {
    vector<int> l1 = {1, 1, 1};
    vector<int> l2 = {1, 1, 1, 1};
    vector<int> l3 = {1, 2, 3};
    vector<int> l4 = {1, 2, 3, 4};
    vector<double> l5 = {1.0, 2.5, 3.0};
    vector<double> l6 = {1.5, 2.0};
    EXPECT_EQ(1, calc_median(l1));
    EXPECT_EQ(1, calc_median(l2));
    EXPECT_EQ(2, calc_median(l3));
    EXPECT_EQ(2, calc_median(l4));
    EXPECT_FLOAT_EQ(2.5, calc_median(l5));
    EXPECT_FLOAT_EQ(1.75, calc_median(l6));
}

TEST_F(MathTest, Range) {
    auto ran = range(0, 10, 1); // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    EXPECT_EQ(ran.size(), 10);
    EXPECT_EQ(ran[0], 0);
    EXPECT_EQ(ran[ran.size()-1], 9);

    auto ran2 = range(0, 30, 5); // 0, 5, 10, 15, 20, 25
    EXPECT_EQ(ran2.size(), 6);
    EXPECT_EQ(ran2[0], 0);
    EXPECT_EQ(ran2[ran2.size()-1], 25);

    auto ran3 = range(0.0, 1.0, 0.1);
    EXPECT_EQ(ran3.size(), 10);
    EXPECT_FLOAT_EQ(0.0, ran3[0]);
    EXPECT_FLOAT_EQ(0.9, ran3[ran3.size()-1]);
}

TEST_F(MathTest, RandomInts) {
    auto gen = get_random_generator();

    // random single int
    int r1 = random_integer(gen, 0, 10);
    int r2 = random_integer(gen, 0, 10);
//    EXPECT_NE(r1, r2);
    EXPECT_GE(r1, 0);
    EXPECT_GE(r2, 0);
    EXPECT_LE(r1, 10);
    EXPECT_LE(r2, 10);

    // random vector
    std::vector<int> rands1 = random_integers(gen, 0, 10000, 10);
    std::vector<int> rands2 = random_integers(gen, 0, 10000, 10);
    EXPECT_EQ(10, rands1.size());

    // Check that all numbers are different
    for (int j = 0; j < rands1.size() - 1; ++j) {
        EXPECT_NE(rands1[j], rands1[j+1]);
    }

    // Check that the two sets of rands are different
    for (int i = 0; i < rands1.size(); ++i) {
        EXPECT_NE(rands1[i], rands2[i]);
    }
}

TEST_F(MathTest, RandomFloats) {
    auto gen = get_random_generator();
    std::vector<float> rands1 = random_floats(gen, 10);
    std::vector<float> rands2 = random_floats(gen, 10);
    EXPECT_EQ(10, rands1.size());

    // Check that all numbers are different
    for (int i = 0; i < rands1.size() - 1; ++i) {
        EXPECT_NE(rands1[i], rands1[i+1]);
    }

    // Check that the two sets of rands are different
    for (int j = 0; j < rands1.size(); ++j) {
        EXPECT_NE(rands1[j], rands2[j]);
    }
}

TEST_F(MathTest, SymmetricBernoulli) {
	auto gen = get_random_generator(1);
	auto numbers = random_symmetric_bernoulli(gen, 15);
	EXPECT_EQ(15, numbers.size());
	for (int number : numbers) {
		EXPECT_TRUE(number == 1 || number == -1);
	}
}

TEST_F(MathTest, SeededRandoms) {
    auto gen11 = get_random_generator(1);
    auto gen12 = get_random_generator(2);
    auto gen21 = get_random_generator(1);
    auto gen22 = get_random_generator(2);

    double rand11 = random_double(gen11);
    double rand12 = random_double(gen12);
    double rand21 = random_double(gen21);
    double rand22 = random_double(gen22);
    EXPECT_EQ(rand11, rand21);
    EXPECT_EQ(rand12, rand22);
    EXPECT_NE(rand11, rand12);
    EXPECT_NE(rand21, rand22);
}

}
