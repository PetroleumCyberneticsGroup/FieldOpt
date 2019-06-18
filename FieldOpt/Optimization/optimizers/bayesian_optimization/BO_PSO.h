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
class BO_PSO : public Optimizer {
public:
    TerminationCondition IsFinished() override;
    BO_PSO(Settings::Optimizer *settings,
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

private:
    VectorXd lb_, ub_; //!< Upper and lower bounds
    int n_initial_guesses_; //!< Number of random cases to be generated initially.
    libgp::GaussianProcess *gp_; //!< The gaussian process to be used throughout the optimization run.
    BayesianOptimization::AcquisitionFunction af_; //!< Acquisition function to be used throughout the optimization run.
    BayesianOptimization::AFOptimizers::AFPSO af_opt_; //!< Aquisition function optimizer to be used throughout the optimization run.
    Settings::Optimizer *settings_;

    long int time_af_opt_;
    long int time_fitting_;
public:
    struct Particle{
        Eigen::VectorXd rea_vars; //!< Real variables
        Case *case_pointer; //!< Pointer to the case
        Eigen::VectorXd rea_vars_velocity; //!< The velocity of the real variables
        Particle(Optimization::Case *c, boost::random::mt19937 &gen, Eigen::VectorXd v_max, int n_vars);
        Particle(){}
        void ParticleAdapt(Eigen::VectorXd rea_vars_velocity_swap, Eigen::VectorXd rea_vars);
        double ofv() { return case_pointer->objective_function_value(); }
    };
    double stagnation_limit_; //!< The stagnation criterion, standard deviation of all particle positions.
    vector<vector<Particle>> swarm_memory_; //!< The memory of the swarm at previous timesteps.
    boost::random::mt19937 gen_; //!< Random number generator with the random functions in math.hpp
    int number_of_particles_; //!< The number of particles in the swarm
    double learning_factor_1_; //!< Learning factor 1 (c1)
    double learning_factor_2_; //!< Learning factor 2 (c2)
    Eigen::VectorXd v_max_; //!< Max velocity of the particle
    int max_iterations_; //!< Max iterations
    vector<Particle> swarm_; //!< Current swarm of particles
    Particle current_best_particle_global_; //!< global best particle position
    Eigen::VectorXd lower_bound_; //!< Lower bounds for the variables (used for randomly generating populations and mutation)
    Eigen::VectorXd upper_bound_; //!< Upper bounds for the variables (used for randomly generating populations and mutation)
    int n_vars_; //!< Number of variables in the problem.
    /*!
* @brief
* Generates a random set of cases within their given upper and lower bounds. The function also generates an initial
* velocity based on the vMax parameter given through the .json file.
* @return
*/
    Case *generateRandomCase();
    /*!
     * @brief Looks through the memory of the swarm in order to find the best evaluated perturbation.
     * @param swarm
     * @param current_best_particle_global
     * @return
     */
    Particle get_global_best();

    /*!
     * @brief Updates the velocity based on learning_factor_1_ (c1), learning_factor_2_ (c2), the best evaluated
     * perturbation of the swarm and the best evaluated perturbation of that particle.
     * @param swarm_memory
     * @return
     */
    vector<BO_PSO::Particle> update_velocity();
    /*!
     * @brief Updates the position based on the updated velocities of the particles in the swarm.
     * @return
     */
    vector<BO_PSO::Particle> update_position();
    /*!
     * @brief Prints the swarm and its current values in a readable format, calls print particle
     * @param swarm
     */
    void printSwarm(vector<Particle> swarm = vector<Particle>()) const;
    /*!
     * @brief Prints the individual particle in a readable format
     * @param partic
     */
    void printParticle(Particle &partic) const;
    /*!
     * @brief Finds the best perturbation in the individual particle's memory
     * @param swarm_memory
     * @param particle_num
     * @return
     */
    Particle find_best_in_particle_memory(int particle_num);
    /*!
     * @brief Performs a check on the swarm, to figure out whether it is stuck with particles that are too close to one
     * another.
     * @return
     */
    bool is_stagnant();

    class ConfigurationSummary : public Loggable {
    public:
        ConfigurationSummary(BO_PSO *opt) { opt_ = opt; }
        LogTarget GetLogTarget() override;
        map<string, string> GetState() override;
        QUuid GetId() override;
        map<string, vector<double>> GetValues() override;

    private:
        BO_PSO *opt_;
    };
};

}
}
}


#endif //FIELDOPT_BO_PSO_H
