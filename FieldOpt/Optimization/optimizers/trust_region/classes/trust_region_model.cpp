#include "trust_region_model.h"

TrustRegionModel::TrustRegionModel(QList<Eigen::VectorXd> points, QList<double> fvalues, double radius, int dimension){
    points_ = points;
    fvalues_ = fvalues;
    center_ = points.at(0);
    radius_ = radius;
    dimension_ = dimension;
    QList<Polynomial> basis;
    for (int i = 0; i < (dimension+1)*(dimension+2)/2; ++i) {
        Eigen::VectorXd temp_vec = Eigen::VectorXd::Zero((dimension+1)*(dimension+2)/2);
        temp_vec(i) = 1;
        Polynomial temp_poly = Polynomial(dimension, temp_vec);
        basis.append(temp_poly);
    }
    basis_ = basis;

}

QList<Eigen::VectorXd> TrustRegionModel::get_points() const{
    return points_;
}

QList<double> TrustRegionModel::get_fvalues() const{
    return fvalues_;
}

double TrustRegionModel::get_radius() const{
    return radius_;
}

QList<Polynomial> TrustRegionModel::get_basis() const{
    return basis_;
}

Eigen::VectorXd TrustRegionModel::get_model_coeffs() const {
    return model_coeffs_;
}

void TrustRegionModel::complete_points() {
    // Complete set of interpolation points so
    // that the set is well-poised

    /* Pivot element tolerance. Note that we can always find
     * an element inside the ball of radius one so that the
     * value is at least 0.25. It might however be clever to
     * lower this tolerance in order to preserve as many stored
     * function evaluations as possible.
     */
    double tol_pivot = 0.20;
    Eigen::VectorXd centre_point = points_.at(0);

    /* scaling points to a ball of radius 1
     * and center at center_ (first point of
     * the points list)
     */
    double max_norm = 0;
    for(int i=1; i<points_.length(); i++){
        Eigen::VectorXd diff = points_.at(i) - centre_point;
        if(diff.norm() > max_norm){
            max_norm = diff.norm();
        }
    }
    std::cout << "max norm = " << max_norm << std::endl;
    if(max_norm==0){
        max_norm = 1;
        std::cout << "all points are the same point or only 1 point in set" << std::endl;
    }

    QList<Eigen::VectorXd> points_abs;
    for (int i = 0; i < points_.length(); ++i) {
        //Eigen::VectorXd diff_vec = (1/max_norm)*(get_points().at(i)-get_points().at(0));//points_.at(i)-points_.at(0);
        points_abs.append((1/max_norm)*(get_points().at(i) - centre_point));
    }

    int n_Polynomials = basis_.length();
    int n_points = points_.length();
    QList<Polynomial> temp_basis = get_basis();

    for(int i=0; i<n_Polynomials; i++){
        Polynomial cur_pol = temp_basis.at(i);
        double max_abs = 0.0;
        int max_abs_ind = -1;
        std::cout << "hello 01 i = "<< i << std::endl;
        for (int j = i; j < n_points; ++j) {
            std::cout << "hello 10 j = "<< j << std::endl;
            //std::cout << "points_abs.at(j) = " << points_abs.at(j) << std::endl;
            //std::cout << cur_pol.evaluate(points_abs.at(j)) << std::endl;
            if(fabs(cur_pol.evaluate(points_abs.at(j)))>max_abs){
                max_abs = fabs(cur_pol.evaluate(points_abs.at(j)));
                max_abs_ind = j;
            }
        }

        /* If evaluation in pivot element is greater than threshold, switch elements
         * and its associated function evaluations
         */
        std::cout << "max_abs_ind = " << max_abs_ind << std::endl;
        if(max_abs>tol_pivot){
            points_abs.swap(i,max_abs_ind);
            fvalues_.swap(i,max_abs_ind);
            std::cout << "YES sufficient pivot element aka. good point" << std::endl;
        }
        else{
            std::cout << "hello 04 i = "<< i << std::endl;
            //Find new point using alg proposed by Conn
            std::cout << "NO sufficient pivot element aka. good point, use CONN alg" << std::endl;
            Polynomial temp_poly_here = temp_basis.at(i);
            //std::cout << find_new_point(temp_poly_here) << std::endl;
            points_abs.append(find_new_point(temp_poly_here));
            points_abs.swap(i,points_abs.length()-1);
            // TODO: Here we need to evaluate the function of the new (but UNSCALED) point and append it!
            fvalues_.append(silly_function(points_abs.at(i)));
            fvalues_.swap(i,points_abs.length()-1);

            //std::cout << "new point found with value = " << temp_poly_here.evaluate(points_abs.at(i)) << std::endl;
        }

        Polynomial temp_i = temp_basis.at(i);
        auto temp_point = points_abs.at(i);


        for (int j = i+1; j <n_points ; j++) {
            Polynomial uj= temp_basis.at(j);
            Polynomial ui = temp_basis.at(i);
            double temp_ratio = uj.evaluate(temp_point)/ui.evaluate(temp_point);
            ui.multiply(-1.0*temp_ratio);
            uj.add(ui);
            temp_basis[j] = uj;
        }
    }

    // Scale points back
    QList<Eigen::VectorXd> points_scaled;
    for (int i = 0; i < n_Polynomials; ++i) {
        points_scaled.append(centre_point + max_norm*points_abs.at(i));
    }

    points_ = points_scaled;
}

Eigen::VectorXd TrustRegionModel::find_new_point(Polynomial poly) const {
// As described by A. Conn, finds a 'good point' for the scaled trust region.
    // This is a copy of C. Giuliani's Matlab code

    int dimension = poly.return_dimension();
    Eigen::VectorXd coeffs = poly.return_coeffs();
    Eigen::VectorXd x0, x1, x2, x3, x4;
    x0 = x1 = x2 = x3 = x4 = Eigen::VectorXd::Zero(dimension);

    // Find largest monomial coefficient (excluding constant term which has already been assigned to first point)
    double max = 0.0;
    int max_coeff = -1;
    std::cout << x0 << std::endl;
    std::cout << "poly.return_no_elements() = " << poly.return_no_elements() << std::endl;
    std::cout << "poly.return_dimension() = " << poly.return_dimension() << std::endl;
    for (int i = 1; i < poly.return_no_elements(); ++i) {
        if(fabs(coeffs(i)) > max) {
            max = fabs(coeffs(i));
            max_coeff = i;
        }
    }
    if(max_coeff==-1){ std::cout << "Good point alg, Problem: all coefficients are zero" << std::endl;}
    std::cout << "max coeff = " << max_coeff<< std::endl;
    std::cout << "dimension = " << dimension << std::endl;
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
        // Mixed term quadratic is larges
        // There is probably a smarter way to do this... oh whatever I'm lazy
        int l,m = 0;
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
    for(int i=0; i<5; i++){
        if(fabs(poly.evaluate(points.at(i)))>=best_value){
            best_point = points.at(i);
            best_value = fabs(poly.evaluate(points.at(i)));
        }
    }
    //std::cout << "Ended finding point" << std::endl << best_point << "and value is = " << best_value << std::endl;

    return best_point;
}

void TrustRegionModel::calculate_model_coeffs() {
    Eigen::MatrixXd M = Eigen::MatrixXd::Zero(basis_.length() ,basis_.length());
    Eigen::VectorXd y = Eigen::VectorXd::Zero(basis_.length());

    std::cout << "hello there" << std::endl;
    // Build Matrix M from basis and functions evaluations

    for (int i = 0; i < basis_.length(); ++i) {
        for (int j = 0; j < basis_.length(); ++j) {
            M(i,j) = basis_.at(j).evaluate(points_.at(i));
        }
        //y(i) = fvalues_.at(i);
        y(i) = silly_function(points_.at(i));
    }
    std::cout << M << std::endl;

    Eigen::VectorXd alpha = M.inverse()*y;
    model_coeffs_ = alpha;

}

double TrustRegionModel::silly_function(Eigen::VectorXd x) {
    return 3+ 4*x(0) + 3*x(1) + x(0)*x(0) + 5*x(1)*x(1) -1*x(0)*x(1);
}
