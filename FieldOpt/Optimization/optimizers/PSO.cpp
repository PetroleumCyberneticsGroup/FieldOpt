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
  gen_ = get_random_generator(settings->parameters().rng_seed);
  //vector<double> velocity_range_(-vMax_,vMax_);

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

  for (int i = 0; i < number_of_particles_; ++i) {
    auto new_case = generateRandomCase();
    swarm_.push_back(Particle(new_case ,gen_, vMax_, n_vars_));
    case_handler_->AddNewCase(new_case);
  }

    if (verbosity_level_ > 1) {
        cout << "Initial ";
        printSwarm();
    }



  /*
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
  */
}

Case *PSO::generateRandomCase() {
    Case *new_case;
    new_case = new Case(GetTentativeBestCase());

  Eigen::VectorXd erands(n_vars_);
  for (int i = 0; i < n_vars_; ++i) {
    erands(i) = random_doubles(gen_, lower_bound_(i), upper_bound_(i), 1)[0];
  }
  new_case->SetRealVarValues(erands);
  return  new_case;
}

PSO::Particle::Particle(Optimization::Case *c, boost::random::mt19937 gen, double vMax_, int n_vars) {
  case_pointer = c;
  rea_vars=c->GetRealVarVector();
  Eigen::VectorXd temp(n_vars);
  for(int i = 0; i < rea_vars.size(); i++){
      temp(i) = random_doubles(gen, -vMax_, vMax_, 1)[0];
  }
  rea_vars_velocity=temp;
}

void PSO::Particle::ParticleAdapt(Eigen::VectorXd rea_vars_velocity_swap, Eigen::VectorXd rea_vars_swap){
    rea_vars=rea_vars_swap;
    case_pointer->SetRealVarValues(rea_vars);
    rea_vars_velocity = rea_vars_velocity_swap;
}

/*
void PSO::Particle::createNewCase() {
    Case *new_case = new Case(case_pointer);
    new_case->SetRealVarValues(rea_vars);
    case_pointer = new_case;
}
*/
/*
vector<double> PSO::getUniformDistribution(double lower_bound,double upper_bound,int number_of_particles_){
    auto gen = get_random_generator(10);
    auto uniform_distribution = random_reals(gen, lower_bound,upper_bound, number_of_particles_);
    return uniform_distribution;
}
*/
Optimizer::TerminationCondition PSO::IsFinished() {
  if (iteration_ < 100) return NOT_FINISHED;
  //else if (case_handler_->QueuedCases().size() > 0) return NOT_FINISHED;
  else return MAX_EVALS_REACHED;
}

void PSO::printSwarm(vector<Particle> swarm) const {
  if (swarm.size() == 0)
    swarm = swarm_;
  cout << "Population:" << endl;
  for (int i = 0; i < swarm.size(); ++i) {
    cout << "\t" << i << "\t";
    printParticle(swarm[i]);
  }
}

void PSO::printParticle(Particle &partic) const {
  printf("%4.2f\t\t", partic.case_pointer->objective_function_value());
  for (int i = 0; i < n_vars_; ++i) {
    printf("%2.4f\t", partic.rea_vars(i));
  }
  cout << endl;
}

PSO::Particle PSO::get_global_best(vector<vector<Optimization::Optimizers::PSO::Particle>> swarm_memory, Particle current_best_particle_global){
    Particle best_particle;
    if(swarm_memory.size() == 1){
        best_particle = swarm_memory[0][0];
    } else {
        best_particle = current_best_particle_global;
    }
    for(int i = 0; i < swarm_memory.size(); i++){
        for(int j = 0; j < swarm_memory[i].size();j++){
            if (swarm_memory[i][j].case_pointer->objective_function_value() > best_particle.case_pointer->objective_function_value()){
                best_particle=swarm_memory[i][j];
            }
        }
    }
    return best_particle;
}

PSO::Particle PSO::find_best_in_particle_memory(vector<vector<Optimization::Optimizers::PSO::Particle>> swarm_memory, int particle_num){
    Particle best_in_particle_memory = swarm_memory[0][particle_num];
    for(int i = 1; i < swarm_memory.size(); i++) {
        if(swarm_memory[i][particle_num].case_pointer->objective_function_value() > best_in_particle_memory.case_pointer->objective_function_value()){
            best_in_particle_memory = swarm_memory[i][particle_num];
        }
    }
    return best_in_particle_memory;
}

vector<PSO::Particle> PSO::update_velocity(vector<Particle> swarm, PSO::Particle current_best_particle_global, vector<vector<Optimization::Optimizers::PSO::Particle>> swarm_memory) {
    vector<Particle> new_swarm;
    for(int i = 0; i < swarm.size(); i++){
        Particle best_in_particle_memory = find_best_in_particle_memory(swarm_memory, i);
        new_swarm.push_back(swarm[i]);
        for(int j = 0; j < n_vars_; j++){
            double velocity_1 = learning_factor_1_ * random_double(gen_, 0, 1) * (best_in_particle_memory.rea_vars(j)-swarm[i].rea_vars(j));
            double velocity_2 = learning_factor_2_ * random_double(gen_, 0, 1) * (current_best_particle_global.rea_vars(j)-swarm[i].rea_vars(j));
            new_swarm[i].rea_vars_velocity(j) = swarm[i].rea_vars_velocity(j) + velocity_1 + velocity_2;
            cout << new_swarm[i].rea_vars_velocity (j) << endl;
            if (new_swarm[i].rea_vars_velocity(j) < -vMax_){
                new_swarm[i].rea_vars_velocity(j) = -vMax_;
            }else if(new_swarm[i].rea_vars_velocity(j) > vMax_){
                new_swarm[i].rea_vars_velocity(j) = vMax_;
            }
        }
    }
    return new_swarm;
}
/*
PSO::Particle PSO::update_best_position(vector<Particle> swarm) {
    Particle best_particle = swarm[0];
    for(int i = 1; i < swarm.size(); i++){
        if(swarm[i].ofv() > best_particle.ofv()){
            best_particle = swarm[i];
        }
    }

}
*/

vector<PSO::Particle> PSO::update_position(vector<Optimization::Optimizers::PSO::Particle> swarm) {
    for(int i = 0; i < swarm.size(); i++){
        for(int j = 0; j < n_vars_; j++){
            swarm[i].rea_vars(j)=swarm[i].rea_vars_velocity(j)+swarm[i].rea_vars(j);
            cout << swarm[i].rea_vars(j) << endl;
            if (swarm[i].rea_vars(j) > upper_bound_[j]){
                swarm[i].rea_vars(j) = upper_bound_[j]-abs(swarm[i].rea_vars(j)-upper_bound_[j]);
                swarm[i].rea_vars_velocity(j) = swarm[i].rea_vars_velocity(j)*-1.0;
            } else if (swarm[i].rea_vars(j) < lower_bound_[j]){
                swarm[i].rea_vars(j) = lower_bound_[j]-abs(swarm[i].rea_vars(j)-lower_bound_[j]);
                swarm[i].rea_vars_velocity(j) = swarm[i].rea_vars_velocity(j)*-1.0;
            }
        }
    }
    return swarm;
}


void PSO::iterate(){
    if(enable_logging_){
        logger_->AddEntry(this);
    }
    swarm_memory_.push_back(swarm_);
    current_best_particle_global_=get_global_best(swarm_memory_, current_best_particle_global_);
    cout << "Current value of best particle: " << current_best_particle_global_.ofv() << endl;
    swarm_ = update_velocity(swarm_, current_best_particle_global_, swarm_memory_);
    swarm_ = update_position(swarm_);
    vector<Particle> next_generation_swarm;
    for (int i = 0; i < number_of_particles_; ++i) {
        auto new_case = generateRandomCase();
        next_generation_swarm.push_back(Particle(new_case, gen_, vMax_, n_vars_));
        next_generation_swarm[i].ParticleAdapt(swarm_[i].rea_vars_velocity, swarm_[i].rea_vars);
    }
    cout << "Before changing: "<< next_generation_swarm[0].rea_vars(0) << endl;
    for(int i = 0; i < number_of_particles_; i++){
        case_handler_->AddNewCase(next_generation_swarm[i].case_pointer);
    }
    swarm_ = next_generation_swarm;
    cout << "After changing: "<< swarm_[0].rea_vars(0) << endl;
    iteration_++;
}




void PSO::handleEvaluatedCase(Case *c) {
    if(isImprovement(c)){
        updateTentativeBestCase(c);
        if (verbosity_level_ > 1) {
            cout << "New best in swarm " << iteration_ << ": "
                 << GetTentativeBestCase()->objective_function_value() << endl;
        }
    }
}



}
}