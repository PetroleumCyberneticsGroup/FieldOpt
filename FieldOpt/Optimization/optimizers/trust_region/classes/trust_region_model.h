#ifndef FIELDOPT_MODEL_H
#define FIELDOPT_MODEL_H

#include <Eigen/Dense>
#include <QList>
#include "polynomial.h"

class TrustRegionModel {

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
    TrustRegionModel(QList<Eigen::VectorXd> points, QList<double> fvalues, double radius, int dimension);
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

#endif //FIELDOPT_MODEL_H
