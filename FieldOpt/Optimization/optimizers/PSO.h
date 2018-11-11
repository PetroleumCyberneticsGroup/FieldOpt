/******************************************************************************
   Created by Brage on 08/11/18.
   Copyright (C) 2018 Brage Strand Kristoffersen <brage_sk@hotmail.com>

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

#ifndef FIELDOPT_PSO_H
#define FIELDOPT_PSO_H
namespace Optimization {
namespace Optimizers {
class PSO : public Optimizer {
 public:
  PSO(Settings::Optimizer *settings,
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
    Eigen::VectorXd rea_vars;
    Case *case_pointer;
    Eigen::VectorXd rea_vars_velocity;
    Particle(Optimization::Case *c, boost::random::mt19937 gen, double vMax_, int n_vars);
    Particle(){}
    void ParticleAdapt(Eigen::VectorXd rea_vars_velocity_swap, Eigen::VectorXd rea_vars);
    double ofv() { return case_pointer->objective_function_value(); }
  };

  Case *generateRandomCase();
  Particle get_global_best(vector<vector<Particle>> swarm, Particle current_best_particle_global);
  vector<PSO::Particle> update_velocity(vector<Optimization::Optimizers::PSO::Particle>, Particle, vector<vector<Optimization::Optimizers::PSO::Particle>> swarm_memory);
  vector<PSO::Particle> update_position(vector<Particle>);
  void printSwarm(vector<Particle> swarm = vector<Particle>()) const;
  void printParticle(Particle &partic) const;
  Particle find_best_in_particle_memory(vector<vector<Optimization::Optimizers::PSO::Particle>> swarm_memory, int particle_num);
  vector<vector<Particle>> swarm_memory_;

  vector<Particle> improved_swarm_;
  int number_of_particles_ = 3; //!< The number of particles in the swarm
  double learning_factor_1_ = 2; //!< Learning factor 1
  double learning_factor_2_ = 2; //!< Learning factor 2
  double vMax_ = 30; //!< Max velocity of the particle
  vector<Particle> swarm_;
  Particle current_best_particle_global_;
  Eigen::VectorXd lower_bound_; //!< Lower bounds for the variables (used for randomly generating populations and mutation)
  Eigen::VectorXd upper_bound_; //!< Upper bounds for the variables (used for randomly generating populations and mutation)
  int n_vars_; //!< Number of variables in the problem.

};
}
}

#endif //FIELDOPT_PSO_H
