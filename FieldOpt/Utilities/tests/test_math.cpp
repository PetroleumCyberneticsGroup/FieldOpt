#include <gtest/gtest.h>
#include <QtCore/QDate>
#include <iostream>
#include "Utilities/math.hpp"

namespace {

    class MathTest : public testing::Test {

    };


    TEST_F(MathTest, Average) {
        QList<int> l1 = {1, 1, 1, 1};
        QList<int> l2 = {2, 2};
        QList<int> l3 = {0};
        QList<int> l4 = {1, 2, 1, 2, 1, 2};
        EXPECT_FLOAT_EQ(1, calc_average(l1));
        EXPECT_FLOAT_EQ(2, calc_average(l2));
        EXPECT_FLOAT_EQ(0, calc_average(l3));
        EXPECT_FLOAT_EQ(1.5, calc_average(l4));
    }

    TEST_F(MathTest, Median) {
        QList<int> l1 = {1, 1, 1};
        QList<int> l2 = {1, 1, 1, 1};
        QList<int> l3 = {1, 2, 3};
        QList<int> l4 = {1, 2, 3, 4};
        QList<double> l5 = {1.0, 2.5, 3.0};
        QList<double> l6 = {1.5, 2.0};
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
