#include <gtest/gtest.h>
#include <QtCore/QDate>
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
}
