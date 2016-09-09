/// This file contains helper methods for working with maths.
#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

#include <QList>
#include <Eigen/Core>

/*!
 * @brief Calculate the average value of the items in the list. The returned value will always be a double.
 * @param list
 * @return The average value of the elements in the list as a double.
 */
template<typename T>
inline double calc_average(const QList<T> list) {
    assert(!list.empty());
    return std::accumulate(list.begin(), list.end(), 0.0) / list.size();
}

/*!
 * @brief Calculate the median of a list. The returned value will have the same type as the elements in the list.
 * If the values are integers and the list has an even number of elements, the result will be floored.
 * @param list
 * @return
 */
template<typename T>
inline T calc_median(QList<T> list) {
    assert(!list.empty());
    std::sort(list.begin(), list.end());
    size_t size = list.size();
    if (size % 2 == 0)
        return (list[size/2 - 1] + list[size/2])/2;
    else
        return list[size/2];
}

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
    Polynomial(int dimension, Eigen::VectorXd coeffs) {
        dimension_ = dimension;
        coeffs_ = coeffs;
        no_elemts_ = (dimension+1)*(dimension+2)/2;
    };

    Eigen::VectorXd return_coeffs() const;
    int return_dimension() const;
    int return_no_elements() const;

    /*!
     * @brief Evaluates polynomial in a given input point.
     * @param evaluation point.
     * @return Polynomial value.
     */
    double evaluate(Eigen::VectorXd point) {
        /* Assume quadratic monomial basis as presented
     * in Caio Giuliani's summary, but with a slight
     * change in ordering. Here the constant term is
     * the first one, then the linear terms, then
     * the quadratic terms of one variable, and finally
     * the mixed terms. i.e. (1, x_1, x_2,..., 0.5*x_N,
     * 0.5*x_1^2, x_2^2,..., 0.5*x_N^2, x_1*x_2, x_1*x_3,
     * ..., x_(N-1)*x_N )
     */

        // CHECK POLYNOMIAL AND POINT DIMENSIONS SAME
        double sum = coeffs_(0);
        for(int i=0; i<dimension_; i++){
            // Linear term and quadratic single term
            sum = sum + coeffs_(i+1)*point(i);
            sum = sum + 0.5*coeffs_(dimension_+i+1)*point(i)*point(i);
        }

        // iter is just an iterator to keep track of where we are in the
        // coefficient vector. Simpler than writing as a function of i and j
        int iter = 2*dimension_+1;

        for(int i=0; i<dimension_-1; i++){
            for(int j=i+1; j<dimension_; j++){
                sum = sum + coeffs_(iter)*point(i)*point(j);
                iter = iter +1;
            }
        }

        return sum;
    };

    Eigen::VectorXd evaluateGradient(Eigen::VectorXd point);
    void add(Polynomial poly);
    void multiply(double k);

};

class PolyModel {

private:
    // Member variabes
    QList<Eigen::VectorXd> points_;
    QList<double> fvalues_;
    Eigen::VectorXd center_;
    double radius_;
    int dimension_;
    // Monomial basis of model, usually quadratic
    QList<Polynomial> basis_;
    // The coefficients of the model using basis
    Eigen::VectorXd model_coeffs_;
    // Private methods/sub-methods
    Eigen::VectorXd find_new_point(Polynomial poly) const;


public:
    PolyModel(QList<Eigen::VectorXd> points, QList<double> fvalues, double radius, int dimension);
    QList<Eigen::VectorXd> get_points() const;
    QList<double> get_fvalues() const;
    double get_radius() const;
    QList<Polynomial> get_basis() const;
    Eigen::VectorXd get_model_coeffs() const;

    /* Complete set of interpolation points
     * using Algorithm 5 as described in paper
     * by C. Giuliani
     */
    void complete_points();

    /* Calculate coefficients of quadratic model
     * of the trust region using a complete and
     * well poised set of points
     */

    void calculate_model_coeffs();

    // Silly function for testing:
    double silly_function(Eigen::VectorXd x);

};

#endif // MATH_FUNCTIONS_H
