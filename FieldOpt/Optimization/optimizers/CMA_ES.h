/******************************************************************************
   Created by Brage on 18/06/19.
   Copyright (C) 2019 Brage Strand Kristoffersen <brage_sk@hotmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef FIELDOPT_CMA_ES_H
#define FIELDOPT_CMA_ES_H

#include <boost/random.hpp>
#include "optimizer.h"

namespace Optimization {
namespace Optimizers {


/*!
* @brief This class implements the "Covariance Matrix Adaption Evolutionary Strategy" (CMA-ES) algorithm.
*
* It samples the space using a multivariate normal distribution in each real-valued dimension. For each step the
* variables in the population is perturbed to achieve a zero mean, based on the covariance matrix. For each step
* the coverance matrix is based on the new distribution around the new mean.
*
* It approximates the
* The implementation is based on the description found at:
* http://delivery.acm.org/10.1145/3080000/3075986/p183-sakamoto.pdf?ip=129.241.230.160&id=3075986&acc=ACTIVE%20SERVICE&key=CDADA77FFDD8BE08%2E5386D6A7D247483C%2E4D4702B0C3E38B35%2E4D4702B0C3E38B35&__acm__=1560861322_554a2416a12a88617c76b0a945b1687c
*
*
*
*
*/
class CMA_ES : public Optimizer {
public:
    CMA_ES(Settings::Optimizer *settings,
           Case *base_case,
           Model::Properties::VariablePropertyContainer *variables,
           Reservoir::Grid::Grid *grid,
           Logger *logger,
           CaseHandler *case_handler=0,
           Constraints::ConstraintHandler *constraint_handler=0);
protected:
    void handleEvaluatedCase(Case *c) override;
    void iterate() override;
    virtual TerminationCondition IsFinished() override;
protected:
    boost::random::mt19937 gen_; //!< Random number generator with the random functions in math.hpp
    std::mt19937 gen;
public:
    struct Individual{
        Eigen::VectorXd rea_vars_; //!< Real variables
        Case *case_pointer_; //!< Pointer to the case
        Eigen::VectorXd erands_norm_; //!< Normalized real variables
        double penalty_dist_; //!< Sum of normalized distance outside of the "distance"
        int index_; //!< index
        Individual(Optimization::Case *c, boost::random::mt19937 &gen, int index, Eigen::VectorXd erands_norm, double penalty_dist);
        Individual(){}
        double ofv() { return case_pointer_->objective_function_value(); }
    };

    Settings::Optimizer *settings_;
    std::random_device rd{};
    std::normal_distribution<> d;
    /*!
     * @brief
     * Generates a set of cases within their given upper and lower bounds, based on the mean (or base case, if utilizied).
     * @return
     */
    Case *generateCase(Eigen::VectorXd xmean, int index, bool first_iteration);

    void updateEvolutionPath(); //!< Updated the Evolution Path
    void adaptCovarianceMatrix(); //!< The adaption of Covariance Matrix (the CMA of CMA-ES)
    void decompositionOfC(); //!< Utilizing the Covariance matrix to update the next meanx.
    vector<Individual> sortPopulation(vector<Individual> population);
    vector<Individual> population_; //!< The storage vector of the population
    vector<Individual> temp_population_; //!< Temporary storage for the new generation that will be merged.
    bool improve_base_case_ = false;
    double stagnation_limit_; //!< The stagnation criterion, standard deviation of all particle positions.
    int population_size_ = -1; //!< The number of people in the population
    double penalty_;
    int max_iterations_; //!< Max iterations
    double sigma_; //!< coordinate wise standard deviation (step size)
    double lambda_; //!< Population size, offspring number
    double mu_; //!< Number of parents/points for recombination
    double mueff_; //!< variance-effectiveness of sum w_i x_i
    double cc_; //!< time constant for cumulation for C
    double cs_; //!< t-const for cumulation for sigma control
    double c1_; //!< Learning rate for rank-one update of C
    double cmu_; //!< and for rank-mu update
    double damps_; //!< damping for sigma usually close to 1
    vector<double> weights_; //!< muXone array for weighted recombination
    double chiN_; //!< expectation of ||N(0,I)|| == norm(randn(N,1))
    bool hsig_;
    Eigen::VectorXd xmean_; //!< The mean of which the normal distribution is generated around
    Eigen::VectorXd xold_;
    Eigen::VectorXd pc_;
    Eigen::VectorXd ps_; //!< evolution paths for C and sigma
    Eigen::VectorXd D_; //!< D defines the scaling
    Eigen::MatrixXd B_; //!< B defines the coordinate system
    Eigen::MatrixXd C_; //!< Co-variance matrix
    Eigen::MatrixXd invsqrtC_; //!< The inverse of the co-variance matrix
    double eigeneval_;
    EigenSolver<MatrixXd> es_; //!< The EigenValueSolver from Eigen, which allows us to calculated the eigenvalues and eigenvector.
    Eigen::VectorXd lower_bound_; //!< Lower bounds for the variables (used for generating populations, and maintaining the search space)
    Eigen::VectorXd upper_bound_; //!< Upper bounds for the variables (used for generating populations, and maintaining the search space)
    int n_vars_; //!< Number of variables in the problem.

};
}
}


#endif //FIELDOPT_CMA_ES_H
