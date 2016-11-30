//
// Created by cutie on 07.10.16.
//

#include "poly_model.h"

PolyModel::PolyModel(Optimization::Case* initial_case, double radius) {
    cases_.append(initial_case);
    points_.append(initial_case->GetRealVarVector());
    center_ = points_.at(0);
    radius_ = radius;
    dimension_ = center_.size();
    QList<Polynomial> basis;
    for (int i = 0; i < (dimension_+1)*(dimension_+2)/2; ++i) {
        Eigen::VectorXd temp_vec = Eigen::VectorXd::Zero((dimension_+1)*(dimension_+2)/2);
        temp_vec(i) = 1;
        Polynomial temp_poly = Polynomial(dimension_, temp_vec);
        basis.append(temp_poly);
    }
    basis_ = basis;

    needs_set_of_points_ = true;

    // Check if objective value of base case has been computed
    if(initial_case->objective_function_value() == std::numeric_limits<double>::max()) {
        needs_evals_ = true;
        cases_not_eval_.append(initial_case);
    }
    else{needs_evals_ = false;}

    is_model_complete_ = false;
}

Optimization::Case* PolyModel::CaseFromPoint(Eigen::VectorXd point, Optimization::Case *prototype) {
    // In order for case to exist outside poly_model, we use the new operator
    Optimization::Case *new_case = new Optimization::Case(prototype);
    new_case->SetRealVarValues(point);
    new_case->set_objective_function_value(std::numeric_limits<double>::max());
    //objective value must be changed to not evalated, i.e. MAXLIMITSDOUBLE

    return new_case;
}

Eigen::VectorXd PolyModel::find_new_point(Polynomial basis_function) {

    int dimension = basis_function.return_dimension();
    Eigen::VectorXd coeffs = basis_function.return_coeffs();
    Eigen::VectorXd x0, x1, x2, x3, x4;
    x0 = x1 = x2 = x3 = x4 = Eigen::VectorXd::Zero(dimension);

    // Find largest monomial coefficient (excluding constant term which has already been assigned to first point)
    double max = 0.0;
    int max_coeff = -1;
    for (int i = 1; i < coeffs.size(); ++i) {
        if(fabs(coeffs(i)) > max) {
            max = fabs(coeffs(i));
            max_coeff = i;
        }
    }

    if(max_coeff==-1){
        std::cout << "Good point alg, Problem: all coefficients are zero, should never happen" << std::endl;
        // LETS JUST PRINT A FEW OF THE COEFFS
        for(int i=0; i<3; i++){
            std::cout << coeffs[i] << std::endl;
        }
    }
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
            for (int j=i+1; j<dimension; j++) {
                if (max_coeff == coeff_dummy) {
                    l = i;
                    m = j;
                    goto endloop;
                }
                coeff_dummy = coeff_dummy+1;
            }
        }
        endloop:

        x1(l) =  1.0/sqrt(2);
        x1(m) = -1.0/sqrt(2);
        x2 = -x1;
        for(int i=0; i<dimension; i++){
            x3(i) = fabs(x1(i));
            x4(i) = -fabs(x1(i));
        }
    }
    Eigen::VectorXd best_point;
    double best_value = 0.0;
    QList<Eigen::VectorXd> points;
    points.append(x0);
    points.append(x1);
    points.append(x2);
    points.append(x3);
    points.append(x4);

    // Determine which of the 5 points is the best one
    for(int i=0; i<5; i++){
        if(fabs(basis_function.evaluate(points.at(i)))>=best_value){
            best_point = points.at(i);
            best_value = fabs(basis_function.evaluate(points.at(i)));
        }
    }

    return best_point;
}

void PolyModel::complete_points() {
    // Complete set of interpolation points so
    // that the set is well-poised

    /* Pivot element tolerance. Note that we can always find
     * an element inside the ball of radius one in order to
     * get as close as we want to 0.25. It might however be clever
     * to lower this tolerance in order to preserve as many stored
     * function evaluations as possible.
     */
    double tol_pivot = 0.10;
    Eigen::VectorXd centre_point = points_.at(0);

    /* scaling points to a ball of radius 1
     * and center at center_ (first point of
     * the points list)
     */
    QList<Eigen::VectorXd> points_abs;
    for (int i = 0; i < points_.length(); ++i) {
        points_abs.append((1/radius_)*(get_points().at(i) - centre_point));
    }

    int n_Polynomials = basis_.length();
    int n_points = points_.length();
    QList<Polynomial> temp_basis = get_basis();

    for (int i = 0; i < n_Polynomials; i++) {
        Polynomial cur_pol = temp_basis.at(i);
        double max_abs = 0.0;
        int max_abs_ind = -1;
        for (int j = i; j < n_points; ++j) {
            /* If new max value, and within a distance radius_ of center point,
             * accept this point as the currently best point.
             * Note that we have scaled the points so we only need to
             * check that the norm of the current points is <=1
             */
            if (fabs(cur_pol.evaluate(points_abs.at(j))) > max_abs && points_abs.at(i).norm() <= 1) {
                max_abs = fabs(cur_pol.evaluate(points_abs.at(j)));
                max_abs_ind = j;
            }
        }

        /* If evaluation in pivot element is greater than threshold,
         * switch elements
         * and its associated function evaluations.
         */
        if (max_abs > tol_pivot) {
            std::cout << "didn't need to find new point, basis polynomial, i = " << i << std::endl;
            //YES sufficient pivot element aka. good point
            points_abs.swap(i, max_abs_ind);
            cases_.swap(i, max_abs_ind);
        }
        else {
            //NO sufficient pivot element aka. good point
            //Find new point using alg proposed by Conn
            Polynomial temp_poly_here = temp_basis.at(i);

            // Append new point and swap it to current position
            points_abs.append(find_new_point(temp_poly_here));
            points_abs.swap(i, points_abs.length() - 1);

            // Make and append new case (remember to scale point back first)
            cases_.append(CaseFromPoint(centre_point + radius_*points_abs.at(i), cases_.at(0)));
            cases_.swap(i, points_abs.length() - 1);

            // Append case to list of unevaluated cases
            cases_not_eval_.append(cases_.at(i));
            needs_evals_ = true;

            //TODO: TEST PRINTING BELOW

            //if(i==-1){
            //    std::cout << "current polynomial = " << std::endl << temp_poly_here.return_coeffs() << std::endl;
            //    std::cout << "found point = " << std::endl << points_abs.at(i) << std::endl;
            //    std::cout << "poly(point) = " << temp_poly_here.evaluate(points_abs.at(i)) << std::endl;
            //}
        }

        Polynomial temp_i = temp_basis.at(i);
        auto temp_point = points_abs.at(i);

        for (int j = i + 1; j < n_Polynomials; j++) {
            Polynomial uj = temp_basis.at(j);
            Polynomial ui = temp_basis.at(i);
            double ratio;
            //TODO: TESTING DIVISION BY ZERO
            if(ui.evaluate(temp_point)==0 && j==i+1){
                std::cout << "Division by zero because U_i(y_i) = 0" << std::endl;}
            // END TESTING
            if(uj.evaluate(temp_point) == ui.evaluate(temp_point)){
                ratio = 1.0;
            }
            else{ratio = uj.evaluate(temp_point) / ui.evaluate(temp_point);}
            ui.multiply(-1.0 * ratio);
            uj.add(ui);
            temp_basis[j] = uj;
        }
    }

    // Scale points back
    QList<Eigen::VectorXd> points_scaled;
    for (int i = 0; i < n_Polynomials; ++i) {
        points_scaled.append(centre_point + radius_*points_abs.at(i));
    }

    needs_set_of_points_ = false;
    points_ = points_abs;
}

void PolyModel::calculate_model_coeffs() {
    if(!needs_evals_ && !needs_set_of_points_){
        Eigen::MatrixXd M = Eigen::MatrixXd::Zero(basis_.length() ,basis_.length());
        Eigen::VectorXd y = Eigen::VectorXd::Zero(basis_.length());


        // Build Matrix M from basis and functions evaluations
        for (int i = 0; i < basis_.length(); ++i) {
            for (int j = 0; j < basis_.length(); ++j) {
                Polynomial current_basis = basis_.at(j);
                M(i,j) = current_basis.evaluate(points_.at(i));
            }
            y(i) = cases_.at(i)->objective_function_value();
        }

        Eigen::VectorXd alpha = M.inverse()*y;
        model_coeffs_ = alpha;
    }
    else{std::cout << "Model_coefficient alg: Either needs evaluations or set of points not finished yet" << std::endl;}
}

void PolyModel::addCenterPoint(Optimization::Case *c) {
    cases_.append(c);
    // Add to list of unevaluated cases if not yet evaluated
    if (c->objective_function_value() == std::numeric_limits<double>::max())
        cases_not_eval_.append(c);
    points_.append(c->GetRealVarVector());
    // Put points in correct position, i.e. first in array
    points_.swap(0,points_.size()-1);
    cases_.swap(0,points_.size()-1);
    center_ = points_.at(0);
    // Set model not yet ready
    is_model_complete_ = false;
}

void PolyModel::optimizationStep() {
    Eigen::VectorXd grad = get_model_coeffs();
    //TODO: use gradient eval function in Polynomial class to get grad, then opt step
}