/******************************************************************************
   Created by igangga on 08/11/20.
   Copyright (C) 2020 I Gusti Agung Gede Angga <i.g.a.g.angga@ntnu.no>

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

#include <boost/random.hpp>
#include "optimizer.h"

#ifndef FIELDOPT_MPSO_H
#define FIELDOPT_MPSO_H
namespace Optimization {
    namespace Optimizers {

        /*!
         * @brief This class implements an extended particle swarm optimization (PSO) algorithm.
         * In this algorithm, there are multiple swarms involved.
         * Each swarm aims to maximize/minimize a specific objective function (marked with a unique ID).
         * Every particle in each swarm computes all objective function values considered.
         * These values are shared/communicated to all swarms, and used to update the global best for each swarm.
         * Each swarm then updates their particles' velocity and position.
         */

        class MPSO : public Optimizer {
        public:
            MPSO(Settings::Optimizer *settings,
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
        public:
            struct Particle{
                Case *case_pointer; //!< Pointer to the case
                Eigen::VectorXd rea_vars; //!< Real variables
                Eigen::VectorXd rea_vars_velocity; //!< The velocity of the real variables
                Particle(Optimization::Case *c, boost::random::mt19937 &gen, Eigen::VectorXd v_max, int n_vars);
                Particle(){}
                void ParticleAdapt(Eigen::VectorXd rea_vars_velocity_swap, Eigen::VectorXd rea_vars);
            };

            /*!
             *
             * @param ObjFn_id
             * @brief
             * Generates a case with a set of random variables within their given upper and lower bounds.
             * The method also sets the case's ObjFn_id. This id indicates to which swarm the case belongs to.
             * @return
             */
            Case *generateRandomCase(const QUuid &ObjFn_id);
            /*!
             * @brief Looks through the memory of the swarm in order to find the best evaluated perturbation.
             * @return
             */
            QHash<QUuid, Particle> get_mpso_id_current_global_best_particle();
            /*!
             * @brief Finds the best perturbation in the individual particle's memory
             * @return
             */
            Particle find_best_in_particle_memory(const QUuid &ObjFn_id, int particle_num);
            /*!
             * @brief Updates the velocity based on learning_factor_1_ (c1), learning_factor_2_ (c2), the best evaluated
             * perturbation of the swarm and the best evaluated perturbation of that particle.
             * @return
             */
            vector<MPSO::Particle> update_velocity(const QUuid &ObjFn_id);
            /*!
             * @brief Updates the position based on the updated velocities of the particles in the swarm.
             * @return
             */
            vector<MPSO::Particle> update_position(const QUuid &ObjFn_id);

            bool isImprovement(const Case *c, const QUuid &ObjFn_id);

            void updateTentativeBestCase(Case *c, const QUuid &ObjFn_id);

            QHash<QUuid, double> mpso_id_r_CO2() override { return mpso_id_r_CO2_; }

            QHash<QUuid, Case *> mpso_id_tentative_best_case() override { return mpso_id_tentative_best_case_; }

            QHash<QUuid, int> mpso_id_tentative_best_case_iteration() override { return mpso_id_tentative_best_case_iteration_; }

            int n_vars_; //!< Number of variables in the problem.
            int max_iterations_; //!< Max iterations
            double learning_factor_1_; //!< Learning factor 1 (c1)
            double learning_factor_2_; //!< Learning factor 2 (c2)
            int number_of_particles_; //!< The number of particles in each swarm
            Eigen::VectorXd lower_bound_; //!< Lower bounds for the variables (used for randomly generating populations and mutation)
            Eigen::VectorXd upper_bound_; //!< Upper bounds for the variables (used for randomly generating populations and mutation)
            Eigen::VectorXd v_max_; //!< Max velocity of the particle
            int mpso_nr_of_swarms_; //!< The number of swarms involved in the optimization.
            QHash<QUuid, double> mpso_id_r_CO2_;
            QHash<QUuid, Case *> mpso_id_tentative_best_case_; //!< Map of the best case encountered thus far from the objective function id.
            QHash<QUuid, int> mpso_id_tentative_best_case_iteration_; //!< Map of the iteration in which the tentative best case was found (for each objective function id).
            QHash<QUuid, vector<vector<Particle>>> mpso_id_swarm_memory_; //!< The memory of the swarm at previous timesteps.
            QHash<QUuid, vector<Particle>> mpso_id_swarm_; //!< Current swarm of particles
            QHash<QUuid, Particle> mpso_id_current_global_best_particle_; //!< global best particle position
        };
    }
}

#endif //FIELDOPT_MPSO_H
