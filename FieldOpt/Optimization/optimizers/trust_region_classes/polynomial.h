//
// Created by cutie on 07.10.16.
//

#ifndef FIELDOPT_POLYNOMIAL_H
#define FIELDOPT_POLYNOMIAL_H

#include <Eigen/Core>
#include <iostream>

/*!
 * @brief The Polynomial class is an implementation to describe second
 * order polynomials with the natural monomial basis of second order polynomials.
 */
class Polynomial {
private:
    int dimension_;
    int no_elemts_;
    Eigen::VectorXd coeffs_;

public:
    /*!
     * @brief Polynomial constructor. Sets coefficients, dimension and calculates number of elements.
     * @param dimension of polynomial.
     * @param An Eigen::VectorXd containing coefficients of polynomial.
     * @return
     */
    Polynomial(int dimension, Eigen::VectorXd coeffs);

    /*!
     * @brief Empty default Polynomial constructor so other classes can contain a Polynomial as private variable
     */
    Polynomial();

    Eigen::VectorXd return_coeffs() {
        return coeffs_;
    };
    int return_dimension() {
        return dimension_;
    };
    int return_no_elements() {
        return no_elemts_;
    };

    /*!
     * @brief Evaluates polynomial in a given input point.
     * @param evaluation point.
     * @return Polynomial value.
     */
    double evaluate(Eigen::VectorXd point);

    /*!
     * @brief Evaluates gradient of model in a given input point.
     * @param gradient evaluation point.
     * @return gradient value.
     */
    Eigen::VectorXd evaluateGradient(Eigen::VectorXd point);

    /*!
     * @brief Adds (the coefficients of) a polynomial to *this polynomial
     * @param Polynomial
     */
    void add(Polynomial poly);

    /*!
     * @brief Multiplies all coefficients of current polynomial with an input double
     * @param Double k
     */
    void multiply(double k);

};



#endif //FIELDOPT_POLYNOMIAL_H
