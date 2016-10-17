//
// Created by cutie on 07.10.16.
//

#ifndef FIELDOPT_POLY_MODEL_H
#define FIELDOPT_POLY_MODEL_H

#include "Optimization/optimizer.h"
#include "polynomial.h"
#include <Eigen/Core>
#include <QList>
#include <iostream>

/*!
 * @brief The PolyModel class is a model for describing a
 * function with polynomials within a given radius of a
 * center point (i.e. trust region). Before the model is
 * computed we must provide a function that is to be
 * approximated and as many function evaluations as we
 * wish. The PolyModel class can then calculate the needed
 * points in order to make the polynomial fitting unique.
 */
class PolyModel {

private:
    // Member variables
    QList<Eigen::VectorXd> points_;
    QList<Optimization::Case*> cases_; // All cases, might or might not be evaluated
    QList<Optimization::Case*> cases_not_eval_; // Cases that need to be sent to case_handler

    // Bools to help trust_region_search class determine next step
    bool needs_evals_; //
    bool needs_set_of_points_;
    bool is_model_complete_; // Bool that is set true whenever a model has been build for current center point and radius

    Eigen::VectorXd center_;
    double radius_;
    int dimension_;
    QList<Polynomial> basis_; // Monomial basis of model, usually quadratic
    Eigen::VectorXd model_coeffs_; // The coefficients of the model using basis

    /*!
             * \brief create a Case from a list of variables and a Case prototype
             *
             * Creates a Case type object from a Case prototype (i.e. a case with the same
             * number of variables but where the variable values have been altered.
             * \return A Case generated from a Eigen::VectorXd point
             */
    Optimization::Case* CaseFromPoint(Eigen::VectorXd point, Optimization::Case *prototype);

    /*!
             * \brief create QList<variables> from a Case
             *
             * Creates a vector which the PolyModel constructor can take as input
             * \return List of Vectors (i.e. positions from given variables) from a Case
             */
    Eigen::VectorXd PointFromCase(Optimization::Case* c);

    /*!
     * @brief As described by A. Conn, finds a 'good point' for the
     * scaled trust region. This is a copy of C. Giuliani's Matlab code
     * @param Double k
     * @return A good point
     */
    Eigen::VectorXd find_new_point(Polynomial poly);

public:
    /*!
     * @brief PolyModel constructor.
     */
    PolyModel(Optimization::Case* base_case, double radius);

    /*!
     * @brief Empty default PolyModel constructor so other classes can contain a PolyModel as private variable
     */
    PolyModel();

    QList<Eigen::VectorXd> get_points() {
        return points_;
    };

    QList<Optimization::Case*> get_cases_not_eval() {
        return cases_not_eval_;
    };

    void ClearCasesNotEval() {
        cases_not_eval_ = QList<Optimization::Case*>();
        needs_evals_ = false;
    }

    bool get_needs_evals() {
        return needs_evals_;
    }

    double get_radius() {
        return radius_;
    };

    QList<Polynomial> get_basis() {
        return basis_;
    };

    Eigen::VectorXd get_model_coeffs() {
        return model_coeffs_;
    };

    bool ModelNeedsEvals() {
        return needs_evals_;
    }

    bool ModelNeedsSetOfPoints() {
        return needs_set_of_points_;
    }

    bool isModelComplete() {
        return is_model_complete_;
    }

    /*!
     * @brief Complete set of interpolation points
     * using Algorithm 5 as described in paper
     * by C. Giuliani
     */
    void complete_points(){
        // Complete set of interpolation points so
        // that the set is well-poised

        /* Pivot element tolerance. Note that we can always find
         * an element inside the ball of radius one in order to
         * get as close as we want to 0.25. It might however be clever
         * to lower this tolerance in order to preserve as many stored
         * function evaluations as possible.
         */
        double tol_pivot = 0.24;
        Eigen::VectorXd centre_point = points_.at(0);

        /* scaling points to a ball of radius 1
         * and center at center_ (first point of
         * the points list)
         */

        /*
        QList<Eigen::VectorXd> points_abs;
        for (int i = 0; i < points_.length(); ++i) {
            points_abs.append((1/radius_)*(get_points().at(i) - centre_point));
        }
        */
        QList<Eigen::VectorXd> points_abs = points_;

        int n_Polynomials = basis_.length();
        int n_points = points_.length();
        QList<Polynomial> temp_basis = get_basis();

        for(int i=0; i<n_Polynomials; i++){
            Polynomial cur_pol = temp_basis.at(i);
            double max_abs = 0.0;
            int max_abs_ind = -1;
            for (int j = i; j < n_points; ++j) {
                /* If new max value, and within a distance radius_ of center point,
                 * accept this point as the currently best point.
                 * Note that we have scaled the points so we only need to
                 * check that the norm of the current points is <=1
                 */
                if(fabs(cur_pol.evaluate(points_abs.at(j)))>max_abs && points_abs.at(i).norm()<=1){
                    max_abs = fabs(cur_pol.evaluate(points_abs.at(j)));
                    max_abs_ind = j;
                }
            }

            /* If evaluation in pivot element is greater than threshold,
             * switch elements
             * and its associated function evaluations.
             */
            if(max_abs>tol_pivot) {
                //YES sufficient pivot element aka. good point
                points_abs.swap(i,max_abs_ind);
                cases_.swap(i,max_abs_ind);
            }
            else{
                //NO sufficient pivot element aka. good point
                //Find new point using alg proposed by Conn
                Polynomial temp_poly_here = temp_basis.at(i);
                // Append new point and swap it to current position
                points_abs.append(find_new_point(temp_poly_here));
                points_abs.swap(i,points_abs.length()-1);
                // Append 0 to signal that point is not yet evaluated
                cases_.append(CaseFromPoint(points_abs.at(i), cases_.at(i)));
                cases_.swap(i,points_abs.length()-1);
                // Append dummy value and change needs_evals_ to true;
                cases_not_eval_.append(CaseFromPoint(points_abs.at(i), cases_.at(i)));
                cases_not_eval_.swap(i,points_abs.length()-1);
                needs_evals_ = true;


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
        /*
        QList<Eigen::VectorXd> points_scaled;
        for (int i = 0; i < n_Polynomials; ++i) {
            points_scaled.append(centre_point + radius_*points_abs.at(i));
        }
        */

        points_ = points_abs;
    };

    /*!
     * @brief Calculate coefficients of quadratic model
     * of the trust region using a complete and
     * well poised set of points
     */
    void calculate_model_coeffs() {
        if(needs_evals_){std::cout << "there are unfinished evaluations, model will be wrong" << std::endl;}

        Eigen::MatrixXd M = Eigen::MatrixXd::Zero(basis_.length() ,basis_.length());
        Eigen::VectorXd y = Eigen::VectorXd::Zero(basis_.length());


        // Build Matrix M from basis and functions evaluations
        for (int i = 0; i < basis_.length(); ++i) {
            for (int j = 0; j < basis_.length(); ++j) {
                Polynomial current_basis = basis_.at(j);
                M(i,j) = current_basis.evaluate(points_.at(i));
            }
            //y(i) = fvalues_.at(i);
            y(i) = cases_.at(i)->objective_function_value();
        }

        Eigen::VectorXd alpha = M.inverse()*y;
        model_coeffs_ = alpha;
        is_model_complete_ = true;

    };

    void add_point(Eigen::VectorXd point){
        points_.append(point);
        needs_evals_ = true;
    }

    void set_evaluations_complete(){
        needs_evals_ = false;
    }


    // Silly function for testing:
    double silly_function(Eigen::VectorXd x) {
        return 3+ 4*x(0) + 3*x(1) + x(0)*x(0) + 5*x(1)*x(1) -1*x(0)*x(1);
    };

};



#endif //FIELDOPT_POLY_MODEL_H
