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
    QList<Optimization::Case*> cases_; //!< All cases, might or might not be evaluated
    QList<Optimization::Case*> cases_not_eval_; //!< Cases that need to be sent to case_handler

    // Bools to help trust_region_search class determine next step
    bool needs_evals_; //
    bool needs_set_of_points_;
    bool is_model_complete_; //!< Bool that is set true whenever a model has been build for current center point and radius

    Eigen::VectorXd center_;
    double radius_;
    int dimension_;
    QList<Polynomial> basis_; //!< Monomial basis of model, usually quadratic
    Eigen::VectorXd model_coeffs_; //!< The coefficients of the model using basis

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
    PolyModel() {};

    /*!
     * \brief create a Case from a list of variables and a Case prototype
     *
     * Creates a Case type object from a Case prototype (i.e. a case with the same
     * number of variables but where the variable values have been altered.
     * \return A Case generated from a Eigen::VectorXd point
     */
    static Optimization::Case* CaseFromPoint(Eigen::VectorXd point, Optimization::Case *prototype);

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
        calculate_model_coeffs();
        return model_coeffs_;
    };

    bool ModelNeedsEvals() {
        return needs_evals_;
    }

    bool ModelNeedsSetOfPoints() {
        return needs_set_of_points_;
    }

    bool isModelReady() {
        if(needs_set_of_points_){return false;}
        else if(needs_evals_){return false;}
        else{return true;}
    }

    /*!
     * @brief Complete set of interpolation points
     * using Algorithm 5 as described in paper
     * by C. Giuliani
     */
    void complete_points();

    /*!
     * @brief Calculate coefficients of quadratic model
     * of the trust region using a complete and
     * well poised set of points
     */
    void calculate_model_coeffs();

    void add_point(Eigen::VectorXd point){
        points_.append(point);
        cases_not_eval_.append(CaseFromPoint(point, cases_.at(0)));
        needs_evals_ = true;
    }

    void set_evaluations_complete(){
        needs_evals_ = false;
    }

    void addCenterPoint(Optimization::Case *c);

};

#endif //FIELDOPT_POLY_MODEL_H
