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
 private:
  struct Particle{
    Eigen::VectorXd rea_vars;
    Case *case_pointer;
    Particle(Case *c);
    Particle() {}
    double ofv() { return case_pointer->objective_function_value(); }
    void createNewCase();
  };

  vector<double> getUniformDistribution(double lower_bound, double upper_bound, int number_of_particles_);
  int number_of_particles_ = 10; //!< The number of particles in the swarm
  Eigen::VectorXd lower_bound_; //!< Lower bounds for the variables (used for randomly generating populations and mutation)
  Eigen::VectorXd upper_bound_; //!< Upper bounds for the variables (used for randomly generating populations and mutation)
  int n_vars_; //!< Number of variables in the problem.
  int max_queue_length_ = 2; //!< Maximum length of queue.
  vector<double> uniform_x_;
  vector<double> uniform_y_;
  vector<double> uniform_z_;
};
}
}

#endif //FIELDOPT_PSO_H
