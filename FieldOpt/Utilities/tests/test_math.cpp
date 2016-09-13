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

    TEST_F(MathTest, Poly_constructor) {
        Eigen::VectorXd coeff(3);
        coeff << 0,1,2;
        Polynomial poly = Polynomial(1,coeff);
        EXPECT_TRUE(true);
    }

    TEST_F(MathTest, poly_values) {
        Eigen::VectorXd pol_0(6), pol_1(6), pol_2(6), pol_3(6), pol_4(6), pol_5(6);
        pol_0 << 1,0,0,0,0,0;
        pol_1 << 0,1,0,0,0,0;
        pol_2 << 0,0,1,0,0,0;
        pol_3 << 0,0,0,1,0,0;
        pol_4 << 0,0,0,0,1,0;
        pol_5 << 0,0,0,0,0,1;

        Polynomial poly_0 = Polynomial(2,pol_0);
        Polynomial poly_1 = Polynomial(2,pol_1);
        Polynomial poly_2 = Polynomial(2,pol_2);
        Polynomial poly_3 = Polynomial(2,pol_3);
        Polynomial poly_4 = Polynomial(2,pol_4);
        Polynomial poly_5 = Polynomial(2,pol_5);
        Eigen::VectorXd point_1(2);
        point_1 << 1, -1;

        EXPECT_EQ(poly_0.evaluate(point_1),1);
        EXPECT_EQ(poly_1.evaluate(point_1),1);
        EXPECT_EQ(poly_2.evaluate(point_1),-1);
        EXPECT_EQ(poly_3.evaluate(point_1),.5);
        EXPECT_EQ(poly_4.evaluate(point_1),.5);
        EXPECT_EQ(poly_5.evaluate(point_1),-1);

    }

    TEST_F(MathTest, gradient_values) {

        // 2-dimensional polynomials
        Eigen::VectorXd pol_0(6), pol_1(6), pol_2(6), grad_0(2), grad_1(2), grad_2(2), point(2);
        pol_0 << 499,0,0,4,0,0;
        pol_1 << -2,-3,43,0,0,0;
        pol_2 << 0,3,0,0,0,12;

        grad_0 << 4,0;
        grad_1 << -3, 43;
        grad_2 << 15, 12;
        point << 1,1;

        Polynomial poly_0 = Polynomial(2,pol_0);
        Polynomial poly_1 = Polynomial(2,pol_1);
        Polynomial poly_2 = Polynomial(2,pol_2);

        // 3-dimensional polynomial
        Eigen::VectorXd pol_3(10), pol_4(10), grad_3(3), grad_4(3), point_2(3);
        pol_3 << 499,0,0,0,4,0,0,0,0,0;
        pol_4 << 0,1,0,0,0,7,0,0,-12,0;
        grad_3 << 4,0,0;
        grad_4 << -11,7,-12;
        point_2 << 1,1,1;

        Polynomial poly_3 = Polynomial(3,pol_3);
        Polynomial poly_4 = Polynomial(3,pol_4);

        EXPECT_TRUE(poly_0.evaluateGradient(point)==grad_0);
        EXPECT_TRUE(poly_1.evaluateGradient(point)==grad_1);
        EXPECT_TRUE(poly_2.evaluateGradient(point)==grad_2);

        EXPECT_TRUE(poly_3.evaluateGradient(point_2)==grad_3);
        EXPECT_TRUE(poly_4.evaluateGradient(point_2)==grad_4);
    }

    TEST_F(MathTest, add_poly) {
        Eigen::VectorXd pol_0(6), pol_1(6), pol_2(6);
        pol_0 <<  1, 0, 0,4,0,-13;
        pol_1 << -2,-3,43,0,0,25;
        pol_2 << -1,-3,43,4,0,12;

        Polynomial poly_0 = Polynomial(2,pol_0);
        Polynomial poly_1 = Polynomial(2,pol_1);
        poly_0.add(poly_1);
        Polynomial poly_2 = Polynomial(2,pol_2);

        EXPECT_TRUE(poly_2.return_coeffs()==poly_0.return_coeffs());
    }
}
