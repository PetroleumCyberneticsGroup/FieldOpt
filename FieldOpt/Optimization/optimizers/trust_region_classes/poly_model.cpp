//
// Created by cutie on 07.10.16.
//

#include "poly_model.h"

PolyModel::PolyModel(Optimization::Case* initial_case, double radius, int dimension) {
    points_cases_.append(initial_case);
    points_.append(); // TODO convert case to variables
    needs_evals_ = false; // All points have been evaluated
    center_ = points.at(0);
    radius_ = radius;
    dimension_ = dimension;
    is_model_complete_ = false;
    QList<Polynomial> basis;
    for (int i = 0; i < (dimension+1)*(dimension+2)/2; ++i) {
        Eigen::VectorXd temp_vec = Eigen::VectorXd::Zero((dimension+1)*(dimension+2)/2);
        temp_vec(i) = 1;
        Polynomial temp_poly = Polynomial(dimension, temp_vec);
        basis.append(temp_poly);
    }
    basis_ = basis;
}

Eigen::VectorXd PolyModel::find_new_point(Polynomial poly) {

    int dimension = poly.return_dimension();
    Eigen::VectorXd coeffs = poly.return_coeffs();
    Eigen::VectorXd x0, x1, x2, x3, x4;
    x0 = x1 = x2 = x3 = x4 = Eigen::VectorXd::Zero(dimension);

    // Find largest monomial coefficient (excluding constant term which has already been assigned to first point)
    double max = 0.0;
    int max_coeff = -1;
    for (int i = 1; i < poly.return_no_elements(); ++i) {
        if(fabs(coeffs(i)) > max) {
            max = fabs(coeffs(i));
            max_coeff = i;
        }
    }
    if(max_coeff==-1){ std::cout << "Good point alg, Problem: all coefficients are zero, should never happen" << std::endl;}
    if(max_coeff<=dimension){
        // The largest coefficient is from a linear term
        x1(max_coeff-1) = 1; //subract 1 maybe if change polynomial form
        x2 = -x1;
    }
    else if(max_coeff<=2*dimension){
        // Largest coefficient is quadratic monomial
        x1(max_coeff-dimension-1) = 1;
        x2 = -x1;
    }
    else{
        // Mixed term quadratic is largest
        // There is probably a smarter way to do this... oh whatever I'm lazy
        int l,m = -1;
        int coeff_dummy = 2*dimension+1;

        for(int i=0; i<dimension-1; i++){
            for (int j=1; j<dimension; j++) {
                if (max_coeff == coeff_dummy) {
                    l = i;
                    m = j;
                    break;
                }
                coeff_dummy = coeff_dummy+1;
            }
        }

        x1(l) =  1.0/sqrt(2);
        x1(m) = -1.0/sqrt(2);
        x2 = -x1;
        for(int i=0; i<dimension; i++){
            x3(i) = fabs(x1(i));
            x4(i) = -fabs(x1(i));
        }
    }
    Eigen::VectorXd best_point;
    double best_value = 0;
    QList<Eigen::VectorXd> points;
    points.append(x0);
    points.append(x1);
    points.append(x2);
    points.append(x3);
    points.append(x4);

    // Determine which of the 5 points is the best one
    for(int i=0; i<5; i++){
        if(fabs(poly.evaluate(points.at(i)))>=best_value){
            best_point = points.at(i);
            best_value = fabs(poly.evaluate(points.at(i)));
        }
    }

    return best_point;
}
