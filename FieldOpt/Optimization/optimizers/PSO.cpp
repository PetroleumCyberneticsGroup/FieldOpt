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
#include "Utilities/random.hpp"
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
    max_iterations_ = settings->parameters().max_generations;
    stagnation_limit_ = settings->parameters().stagnation_limit;
    learning_factor_1_ = settings->parameters().pso_learning_factor_1;
    learning_factor_2_ = settings->parameters().pso_learning_factor_2;
    number_of_particles_ = settings->parameters().pso_swarm_size;
    base_case_ = base_case;
    if (constraint_handler_->HasBoundaryConstraints()) {
        lower_bound_ = constraint_handler_->GetLowerBounds(base_case->GetRealVarIdVector());
        upper_bound_ = constraint_handler_->GetUpperBounds(base_case->GetRealVarIdVector());
    }
    else {
        lower_bound_.resize(n_vars_);
        upper_bound_.resize(n_vars_);
        lower_bound_.fill(settings->parameters().lower_bound);
        upper_bound_.fill(settings->parameters().upper_bound);
    }
    auto difference = upper_bound_ - lower_bound_;
    v_max_ = difference * settings->parameters().pso_velocity_scale;
    if (VERB_OPT > 2) {
        stringstream ss;
        ss << "Using bounds from constraints: " << endl;
        ss << vec_to_str(vector<double>(lower_bound_.data(), lower_bound_.data() + lower_bound_.size()));
        ss << endl;
        ss << vec_to_str(vector<double>(upper_bound_.data(), upper_bound_.data() + upper_bound_.size()));
        Printer::ext_info(ss.str(), "Optimization","PSO");
    }
    for (int i = 0; i < number_of_particles_; ++i) {
        auto new_case = generateRandomCase();
        swarm_.push_back(Particle(new_case ,gen_, v_max_, n_vars_));
        case_handler_->AddNewCase(new_case);
    }
    if (VERB_OPT > 2) {
        printSwarm();
    }
}

void PSO::iterate(){
    if(enable_logging_){
        logger_->AddEntry(this);
    }

    swarm_memory_.push_back(swarm_);
    current_best_particle_global_=get_global_best();
    swarm_ = update_velocity();
    swarm_ = update_position();
    vector<Particle> next_generation_swarm;
    for (int i = 0; i < number_of_particles_; ++i) {
        auto new_case = generateRandomCase();
        next_generation_swarm.push_back(Particle(new_case, gen_, v_max_, n_vars_));
        next_generation_swarm[i].ParticleAdapt(swarm_[i].rea_vars_velocity, swarm_[i].rea_vars);
    }
    for(int i = 0; i < number_of_particles_; i++){
        case_handler_->AddNewCase(next_generation_swarm[i].case_pointer);
    }
    swarm_ = next_generation_swarm;
    iteration_++;
}

void PSO::handleEvaluatedCase(Case *c) {
    if(isImprovement(c)){
        updateTentativeBestCase(c);
        if (VERB_OPT > 1) {
            stringstream ss;
            ss.precision(6);
            ss << scientific;
            ss << "New best in swarm, iteration " << Printer::num2str(iteration_) << ": OFV " << c->objective_function_value();
            Printer::ext_info(ss.str(), "Optimization", "PSO");
        }
    }
}

Optimizer::TerminationCondition PSO::IsFinished() {
    if (case_handler_->CasesBeingEvaluated().size() > 0) return NOT_FINISHED;
    if (is_stagnant()) return MINIMUM_STEP_LENGTH_REACHED;
    if (iteration_ < max_iterations_) return NOT_FINISHED;
    else return MAX_EVALS_REACHED;
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
PSO::Particle::Particle(Optimization::Case *c, boost::random::mt19937 &gen, VectorXd v_max, int n_vars) {
    case_pointer = c;
    rea_vars=c->GetRealVarVector();
    Eigen::VectorXd temp(n_vars);
    for(int i = 0; i < rea_vars.size(); i++){
        temp(i) = random_doubles(gen, -v_max(i), v_max(i), 1)[0];
    }
    rea_vars_velocity=temp;
}
void PSO::Particle::ParticleAdapt(Eigen::VectorXd rea_vars_velocity_swap, Eigen::VectorXd rea_vars_swap){
    rea_vars=rea_vars_swap;
    case_pointer->SetRealVarValues(rea_vars);
    rea_vars_velocity = rea_vars_velocity_swap;
}
void PSO::printSwarm(vector<Particle> swarm) const {
    if (swarm.size() == 0)
        swarm = swarm_;
    stringstream ss;
    ss.precision(6);
    ss << scientific;
    ss << "Population:|" << endl;
    for(auto partic : swarm){
        ss << "OFV: " << partic.ofv() << " Position: ";
        ss << eigenvec_to_str(partic.rea_vars) << "|";
    }
    Printer::ext_info(ss.str(), "Optimization", "PSO");
}
bool PSO::is_stagnant() {
    vector<double> list_of_sums;
    for (auto chrom : swarm_) {
        list_of_sums.push_back(chrom.rea_vars.sum());
    }
    double stdev = calc_standard_deviation(list_of_sums);
    return stdev <= stagnation_limit_;
}
void PSO::printParticle(Particle &partic) const {
}


PSO::Particle PSO::get_global_best(){
    Particle best_particle;
    if(swarm_memory_.size() == 1){
        best_particle = swarm_memory_[0][0];
    } else {
        best_particle = current_best_particle_global_;
    }

    for(int i = 0; i < swarm_memory_.size(); i++){
        for(int j = 0; j < swarm_memory_[i].size();j++){
            if (isBetter(swarm_memory_[i][j].case_pointer, best_particle.case_pointer)) {
                best_particle=swarm_memory_[i][j];
            }
        }
    }
    if(isBetter(base_case_, best_particle.case_pointer)){
        best_particle.rea_vars = base_case_->GetRealVarVector();
    }
    return best_particle;
}
PSO::Particle PSO::find_best_in_particle_memory(int particle_num){
    Particle best_in_particle_memory = swarm_memory_[0][particle_num];
    for(int i = 1; i < swarm_memory_.size(); i++) {
        if (isBetter(swarm_memory_[i][particle_num].case_pointer, best_in_particle_memory.case_pointer)) {
            best_in_particle_memory = swarm_memory_[i][particle_num];
        }
    }
    return best_in_particle_memory;
}
vector<PSO::Particle> PSO::update_velocity() {
    vector<Particle> new_swarm;
    for(int i = 0; i < swarm_.size(); i++){
        Particle best_in_particle_memory = find_best_in_particle_memory(i);
        new_swarm.push_back(swarm_[i]);
        for(int j = 0; j < n_vars_; j++){
            double velocity_1 = learning_factor_1_ * random_double(gen_, 0, 1) * (best_in_particle_memory.rea_vars(j)-swarm_[i].rea_vars(j));
            double velocity_2 = learning_factor_2_ * random_double(gen_, 0, 1) * (current_best_particle_global_.rea_vars(j)-swarm_[i].rea_vars(j));
            new_swarm[i].rea_vars_velocity(j) = swarm_[i].rea_vars_velocity(j) + velocity_1 + velocity_2;
            if (new_swarm[i].rea_vars_velocity(j) < -v_max_(j)){
                new_swarm[i].rea_vars_velocity(j) = -v_max_(j);
            }else if(new_swarm[i].rea_vars_velocity(j) > v_max_(j)){
                new_swarm[i].rea_vars_velocity(j) = v_max_(j);
            }
        }
    }
    return new_swarm;
}
vector<PSO::Particle> PSO::update_position() {
    for(int i = 0; i < swarm_.size(); i++){
        for(int j = 0; j < n_vars_; j++){
            swarm_[i].rea_vars(j)=swarm_[i].rea_vars_velocity(j)+swarm_[i].rea_vars(j);
            if (swarm_[i].rea_vars(j) > upper_bound_[j]){
                swarm_[i].rea_vars(j) = upper_bound_[j]-abs(swarm_[i].rea_vars(j)-upper_bound_[j])*0.5;
                swarm_[i].rea_vars_velocity(j) = swarm_[i].rea_vars_velocity(j)*-0.5;
            } else if (swarm_[i].rea_vars(j) < lower_bound_[j]){
                swarm_[i].rea_vars(j) = lower_bound_[j]+abs(swarm_[i].rea_vars(j)-lower_bound_[j])*0.5;
                swarm_[i].rea_vars_velocity(j) = swarm_[i].rea_vars_velocity(j)*-0.5;
            }
        }
    }
    return swarm_;
}


}
}
