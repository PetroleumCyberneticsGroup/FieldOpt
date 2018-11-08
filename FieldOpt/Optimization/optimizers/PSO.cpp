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

#include "PSO.h"
#include "Utilities/math.hpp"
#include "Utilities/stringhelpers.hpp"
#include <math.h>

namespace Optimization{
namespace Optimizers{
PSO::PSO(Settings::Optimizer *settings,
         Case *base_case,
         Model::Properties::VariablePropertyContainer *variables,
         Reservoir::Grid::Grid *grid,
         Logger *logger,
         CaseHandler *case_handler,
         Constraints::ConstraintHandler *constraint_handler
) : Optimizer(settings, base_case, variables, grid, logger, case_handler, constraint_handler) {

  n_vars_ = variables->ContinousVariableSize();
  max_queue_length_ = settings->parameters().max_queue_size;

  if (constraint_handler_->HasBoundaryConstraints()) {
    lower_bound_ = constraint_handler_->GetLowerBounds(base_case->GetRealVarIdVector());
    upper_bound_ = constraint_handler_->GetUpperBounds(base_case->GetRealVarIdVector());
    if (verbosity_level_ > 1) {
      cout << "Using bounds from constraints: " << endl;
      cout << vec_to_str(vector<double>(lower_bound_.data(), lower_bound_.data() + lower_bound_.size()));
      cout << endl;
      cout << vec_to_str(vector<double>(upper_bound_.data(), upper_bound_.data() + upper_bound_.size()));
      cout << endl;
    }
  }
  else {
    lower_bound_.resize(n_vars_);
    upper_bound_.resize(n_vars_);
    lower_bound_.fill(settings->parameters().lower_bound);
    upper_bound_.fill(settings->parameters().upper_bound);
  }

  double lower_x_value_=524302.86;
  double lower_y_value_=6180070.603;
  double lower_z_value_=2052.826;

  double upper_x_value_=526519.714;
  double upper_y_value_=6180329.13;
  double upper_z_value_=2288.38;

  auto uniform_x_=getUniformDistribution(lower_x_value_,upper_x_value_,number_of_particles_);
  auto uniform_y_=getUniformDistribution(lower_y_value_,upper_y_value_,number_of_particles_);
  auto uniform_z_=getUniformDistribution(lower_z_value_,upper_z_value_,number_of_particles_);

  cout << "Printing out lower bound " << lower_bound_[1] << " Printing out lower bound " << lower_bound_[0] << " Printing out lower bound " << lower_bound_[3] << endl;
  cout << "Printing out upper bound " << upper_bound_[1] << " Printing out upper bound " << upper_bound_[0] << " Printing out upper bound " << upper_bound_[3] << endl;

 //auto uniform_distribution = getUniformDistribution(lower_bound, upper_bound, number_of_particles_);

}
vector<double> PSO::getUniformDistribution(double lower_bound,double upper_bound,int number_of_particles_){
    auto gen = get_random_generator(10);
    auto uniform_distribution = random_reals(gen, lower_bound,upper_bound, number_of_particles_);
    return uniform_distribution;
  }

Optimizer::TerminationCondition PSO::IsFinished() {
  if (iteration_ == 0) return NOT_FINISHED;
  else if (case_handler_->QueuedCases().size() > 0) return NOT_FINISHED;
  else return MAX_EVALS_REACHED;
}

void PSO::handleEvaluatedCase(Case *c) {
}

void PSO::iterate(){

}




}
}