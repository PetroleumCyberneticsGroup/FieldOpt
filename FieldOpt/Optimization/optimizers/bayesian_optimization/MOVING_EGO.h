/******************************************************************************
   Created by Brage on 14/06/19.
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

#ifndef FIELDOPT_BO_PSO_H
#define FIELDOPT_BO_PSO_H

#include "Optimization/optimizer.h"
#include "gp/gp.h"
#include "AcquisitionFunction.h"
#include "af_optimizers/AFPSO.h"
#include <random>


namespace Optimization {
namespace Optimizers {
namespace BayesianOptimization {

/*!
* @brief This class is an implementation of Efficient Global Optimization (EGO),
* i.e. Bayesian Optimization using Gaussian Process models applied to derivative-
* free optimization.
*
* \todo Hyperparameter optimization: after N cases, optimize the GP hyperparameters.
* \todo Convergence criterion: total squared error in model.
* \todo Convergence criterion: Combination of highest expected value ans total squared uncertainty?
*/
    class MOVING_EGO : public Optimizer {
    public:
        TerminationCondition IsFinished() override;
        MOVING_EGO(Settings::Optimizer *settings,
               Case *base_case,
               Model::Properties::VariablePropertyContainer *variables,
               Reservoir::Grid::Grid *grid,
               Logger *logger,
               CaseHandler *case_handler=0,
               Constraints::ConstraintHandler *constraint_handler=0
        );

    protected:
        void handleEvaluatedCase(Case *c) override;
        void iterate() override;
        boost::random::mt19937 gen_; //!< Random number generator with the random functions in math.hpp
        std::mt19937 gen;
    public:
        struct Individual{
            Eigen::VectorXd rea_vars_; //!< Real variables
            Case *case_pointer_; //!< Pointer to the case
            Eigen::VectorXd erands_norm_; //!< Normalized real variables
            double penalty_dist_;
            int index_; //!< index
            Individual(Optimization::Case *c, boost::random::mt19937 &gen, int index, Eigen::VectorXd erands_norm, double penalty_dist);
            Individual(){}
            double ofv() { return case_pointer_->objective_function_value(); }
        };

    private:
        VectorXd lb_, ub_; //!< Upper and lower bounds
        VectorXd clb_, cub_; //!< "Current" upper and lower bounds
        int n_initial_guesses_; //!< Number of random cases to be generated initially.
        libgp::GaussianProcess *gp_; //!< The gaussian process to be used throughout the optimization run.
        BayesianOptimization::AcquisitionFunction af_; //!< Acquisition function to be used throughout the optimization run.
        BayesianOptimization::AFOptimizers::AFPSO af_opt_; //!< Acquisition function optimizer to be used throughout the optimization run.
        Settings::Optimizer *settings_;

        long int time_af_opt_;
        long int time_fitting_;
    public:
        Case *generateCase(Eigen::VectorXd xmean, int index, bool first_iteration);
        Case *generateSpecificCase(Eigen::VectorXd xmean, int index, bool first_iteration);
        void updateEvolutionPath();
        void adaptCovarianceMatrix();
        void decompositionOfC();
        double penalty_ = 1.1;
        std::random_device rd{};
        std::normal_distribution<> d;
        vector<Individual> sortPopulation(vector<Individual> population);
        vector<Individual> population_;
        vector<Individual> temp_population_;
        int max_iterations_; //!< Max iterations
        Eigen::VectorXd lower_bound_; //!< Lower bounds for the variables (used for randomly generating populations and mutation)
        Eigen::VectorXd upper_bound_; //!< Upper bounds for the variables (used for randomly generating populations and mutation)
        int n_vars_; //!< Number of variables in the problem.
        bool improve_base_case_ = true;
        double eigeneval_;
        EigenSolver<MatrixXd> es_;
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
        Eigen::VectorXd xmean_;
        Eigen::VectorXd xold_;
        Eigen::VectorXd pc_;
        Eigen::VectorXd ps_; //!< evolution paths for C and sigma
        Eigen::VectorXd D_; //!< D defines the scaling
        Eigen::MatrixXd B_; //!< B defines the coordinate system
        Eigen::MatrixXd C_; //!< Co-variance matrix
        Eigen::MatrixXd invsqrtC_; //!< Co-variance matrix




    class ConfigurationSummary : public Loggable {
    public:
        ConfigurationSummary(MOVING_EGO *opt) { opt_ = opt; }
        LogTarget GetLogTarget() override;
        map<string, string> GetState() override;
        QUuid GetId() override;
        map<string, vector<double>> GetValues() override;

    private:
        MOVING_EGO *opt_;
    };
};

}
}
}


#endif //FIELDOPT_BO_PSO_H
