/******************************************************************************
   Created by einar on 2/27/19.
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "Utilities/random.hpp"
#include <vector>

using namespace std;

namespace {

class RandomTest : public testing::Test {

};


TEST_F(RandomTest, RandomInts) {
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

TEST_F(RandomTest, RandomFloats) {
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

TEST_F(RandomTest, SymmetricBernoulli) {
    auto gen = get_random_generator(1);
    auto numbers = random_symmetric_bernoulli(gen, 15);
    EXPECT_EQ(15, numbers.size());
    for (int number : numbers) {
        EXPECT_TRUE(number == 1 || number == -1);
    }
}

TEST_F(RandomTest, SeededRandomDouble) {
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

TEST_F(RandomTest, SeededRandomFloats) {
    auto gen1_A = get_random_generator(1);
    auto gen1_B = get_random_generator(1);
    auto gen2_A = get_random_generator(2);
    auto gen2_B = get_random_generator(2);

    int N = 20;

    std::vector<float> rands1_A_1 = random_floats(gen1_A, N);
    std::vector<float> rands1_B_1 = random_floats(gen1_B, N);
    std::vector<float> rands2_A_1 = random_floats(gen2_A, N);
    std::vector<float> rands2_B_1 = random_floats(gen2_B, N);
    std::vector<float> rands1_A_2 = random_floats(gen1_A, N);
    std::vector<float> rands1_B_2 = random_floats(gen1_B, N);
    std::vector<float> rands2_A_2 = random_floats(gen2_A, N);
    std::vector<float> rands2_B_2 = random_floats(gen2_B, N);

    for (int i = 0; i < N; ++i) {
        EXPECT_FLOAT_EQ(rands1_A_1[i], rands1_B_1[i]);
        EXPECT_FLOAT_EQ(rands2_A_1[i], rands2_B_1[i]);
        EXPECT_FLOAT_EQ(rands1_A_2[i], rands1_B_2[i]);
        EXPECT_FLOAT_EQ(rands2_A_2[i], rands2_B_2[i]);
        EXPECT_NE(rands1_A_1[i], rands2_A_1[i]);
        EXPECT_NE(rands1_A_1[i], rands1_A_2[i]);
    }
}


TEST_F(RandomTest, SeededRandomDoubles) {
    auto gen1_A = get_random_generator(1);
    auto gen1_B = get_random_generator(1);
    auto gen2_A = get_random_generator(2);
    auto gen2_B = get_random_generator(2);

    int N = 20;

    std::vector<double> rands1_A_1 = random_doubles(gen1_A, 0, 10, N);
    std::vector<double> rands1_B_1 = random_doubles(gen1_B, 0, 10, N);
    std::vector<double> rands2_A_1 = random_doubles(gen2_A, 0, 10, N);
    std::vector<double> rands2_B_1 = random_doubles(gen2_B, 0, 10, N);
    std::vector<double> rands1_A_2 = random_doubles(gen1_A, 0, 10, N);
    std::vector<double> rands1_B_2 = random_doubles(gen1_B, 0, 10, N);
    std::vector<double> rands2_A_2 = random_doubles(gen2_A, 0, 10, N);
    std::vector<double> rands2_B_2 = random_doubles(gen2_B, 0, 10, N);

    for (int i = 0; i < N; ++i) {
        EXPECT_FLOAT_EQ(rands1_A_1[i], rands1_B_1[i]);
        EXPECT_FLOAT_EQ(rands2_A_1[i], rands2_B_1[i]);
        EXPECT_FLOAT_EQ(rands1_A_2[i], rands1_B_2[i]);
        EXPECT_FLOAT_EQ(rands2_A_2[i], rands2_B_2[i]);
        EXPECT_NE(rands1_A_1[i], rands2_A_1[i]);
        EXPECT_NE(rands1_A_1[i], rands1_A_2[i]);
    }
}

TEST_F(RandomTest, SeededRandomDoublesEigen) {
    auto gen1_A = get_random_generator(1);
    auto gen1_B = get_random_generator(1);
    auto gen2_A = get_random_generator(2);
    auto gen2_B = get_random_generator(2);

    int N = 20;

    Eigen::VectorXd rands1_A_1 = random_doubles_eigen(gen1_A, 0, 10, N);
    Eigen::VectorXd rands1_B_1 = random_doubles_eigen(gen1_B, 0, 10, N);
    Eigen::VectorXd rands2_A_1 = random_doubles_eigen(gen2_A, 0, 10, N);
    Eigen::VectorXd rands2_B_1 = random_doubles_eigen(gen2_B, 0, 10, N);
    Eigen::VectorXd rands1_A_2 = random_doubles_eigen(gen1_A, 0, 10, N);
    Eigen::VectorXd rands1_B_2 = random_doubles_eigen(gen1_B, 0, 10, N);
    Eigen::VectorXd rands2_A_2 = random_doubles_eigen(gen2_A, 0, 10, N);
    Eigen::VectorXd rands2_B_2 = random_doubles_eigen(gen2_B, 0, 10, N);

    for (int i = 0; i < N; ++i) {
        EXPECT_FLOAT_EQ(rands1_A_1[i], rands1_B_1[i]);
        EXPECT_FLOAT_EQ(rands2_A_1[i], rands2_B_1[i]);
        EXPECT_FLOAT_EQ(rands1_A_2[i], rands1_B_2[i]);
        EXPECT_FLOAT_EQ(rands2_A_2[i], rands2_B_2[i]);
        EXPECT_NE(rands1_A_1[i], rands2_A_1[i]);
        EXPECT_NE(rands1_A_1[i], rands1_A_2[i]);
    }
}

}
