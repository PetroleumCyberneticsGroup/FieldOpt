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


}
