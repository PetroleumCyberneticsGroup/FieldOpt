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

#include "MPSO.h"
#include "Utilities/math.hpp"
#include "Utilities/random.hpp"
#include "Utilities/stringhelpers.hpp"
#include <math.h>

namespace Optimization{
    namespace Optimizers{
        MPSO::MPSO(Settings::Optimizer *settings,
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
            learning_factor_1_ = settings->parameters().pso_learning_factor_1;
            learning_factor_2_ = settings->parameters().pso_learning_factor_2;
            number_of_particles_ = settings->parameters().pso_swarm_size;

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
                Printer::ext_info(ss.str(), "Optimization","MPSO");
            }

            std::cout<<"###################################################################################"<<std::endl;
            std::cout<<"Print variable's lower and upper bound; class Optimization/Optimizers/MPSO"<<std::endl;
            QList<QUuid> RealVarIdVector = base_case->GetRealVarIdVector();
            QHash<QUuid, string> variables_name = base_case->variables_name();
            for (int i = 0; i < RealVarIdVector.size(); ++i) {
                QUuid RealVarId = RealVarIdVector.value(i);
                string RealVarName = variables_name.value(RealVarId);
                double RealVarLowBound = lower_bound_(i);
                double RealVarUpBound = upper_bound_(i);
                std::cout<<"RealVarId: ... "<<RealVarId.toString().toStdString()<<std::endl;
                std::cout<<"RealVarName: ... "<<RealVarName<<std::endl;
                std::cout<<"RealVarLowBound: ... "<<RealVarLowBound<<std::endl;
                std::cout<<"RealVarUpBound: ... "<<RealVarUpBound<<std::endl;
            }

            mpso_nr_of_swarms_ = settings->parameters().mpso_nr_of_swarms;
            mpso_id_r_CO2_ = base_case->mpso_id_r_CO2();

            //! Initialize the tentative best case for each swarm
            mpso_id_tentative_best_case_ = QHash<QUuid, Case *> ();
            mpso_id_tentative_best_case_iteration_ = QHash<QUuid, int> ();
            QList<QUuid> ObjFn_ids = mpso_id_r_CO2_.keys();
            for (int i = 0; i < mpso_nr_of_swarms_; ++i) {
                QUuid ObjFn_id = ObjFn_ids[i];
                mpso_id_tentative_best_case_.insert(ObjFn_id, base_case);
                mpso_id_tentative_best_case_iteration_.insert(ObjFn_id, 0);
            }

            //! Initialize the swarm memory for each swarm
            mpso_id_swarm_memory_ = QHash<QUuid, vector<vector<Particle>>> ();
            for (int i = 0; i < mpso_nr_of_swarms_; ++i) {
                QUuid ObjFn_id = ObjFn_ids[i];
                vector<vector<Particle>> swarm_memory;
                mpso_id_swarm_memory_.insert(ObjFn_id, swarm_memory);
            }

            //! Initialize the particles in each swarm
            mpso_id_swarm_ = QHash<QUuid, vector<Particle>> ();
            for (int i = 0; i < mpso_nr_of_swarms_; ++i) {
                QUuid ObjFn_id = ObjFn_ids[i];
                vector<Particle> swarm = vector<Particle> ();
                for (int j = 0; j < number_of_particles_; ++j) {
                    Case *new_case;
                    new_case = generateRandomCase(ObjFn_id);
                    swarm.push_back(Particle(new_case, gen_, v_max_, n_vars_));
                    case_handler_->AddNewCase(new_case);
                }
                mpso_id_swarm_.insert(ObjFn_id, swarm);
            }
        }

        void MPSO::iterate(){
            if(enable_logging_){
                logger_->AddEntry(this);
            }

            //! Append the previous swarm to the swarm memory
            QList<QUuid> ObjFn_ids = mpso_id_r_CO2_.keys();
            for (int i = 0; i < mpso_nr_of_swarms_; ++i) {
                QUuid ObjFn_id = ObjFn_ids[i];
                vector<Particle> swarm = mpso_id_swarm_.value(ObjFn_id);
                vector<vector<Particle>> swarm_memory = mpso_id_swarm_memory_.value(ObjFn_id);
                swarm_memory.push_back(swarm);
                mpso_id_swarm_memory_.insert(ObjFn_id, swarm_memory);
            }

            mpso_id_current_global_best_particle_ = get_mpso_id_current_global_best_particle();

            //! Update particle's velocity (in each swarm)
            for (int i = 0; i < mpso_nr_of_swarms_; ++i) {
                QUuid ObjFn_id = ObjFn_ids[i];
                vector<Particle> swarm = update_velocity(ObjFn_id);
                mpso_id_swarm_.insert(ObjFn_id, swarm);
            }

            //! Update particle's position (in each swarm)
            for (int i = 0; i < mpso_nr_of_swarms_; ++i) {
                QUuid ObjFn_id = ObjFn_ids[i];
                vector<Particle> swarm = update_position(ObjFn_id);
                mpso_id_swarm_.insert(ObjFn_id, swarm);
            }

            QHash<QUuid, vector<Particle>> mpso_id_next_generation_swarm;
            for (int i = 0; i < mpso_nr_of_swarms_; ++i) {
                QUuid ObjFn_id = ObjFn_ids[i];
                vector<Particle> swarm = mpso_id_swarm_.value(ObjFn_id);
                vector<Particle> next_generation_swarm = vector<Particle> ();
                for (int j = 0; j < number_of_particles_; ++j) {
                    Case *new_case;
                    new_case = generateRandomCase(ObjFn_id);
                    next_generation_swarm.push_back(Particle(new_case, gen_, v_max_, n_vars_));
                    next_generation_swarm[j].ParticleAdapt(swarm[j].rea_vars_velocity, swarm[j].rea_vars);
                    case_handler_->AddNewCase(next_generation_swarm[j].case_pointer);
                }
                mpso_id_next_generation_swarm.insert(ObjFn_id, next_generation_swarm);
            }
            mpso_id_swarm_ = mpso_id_next_generation_swarm;
            iteration_++;
        }

        void MPSO::handleEvaluatedCase(Case *c) {
            QList<QUuid> ObjFn_ids = mpso_id_r_CO2_.keys();
            for (int i = 0; i < mpso_nr_of_swarms_; ++i) {
                QUuid ObjFn_id = ObjFn_ids[i];
                if (isImprovement(c, ObjFn_id)) {
                    updateTentativeBestCase(c, ObjFn_id);
                    std::cout<<"Update tentative best case for the swarm with r_CO2 = "<<mpso_id_r_CO2_.value(ObjFn_id)<<std::endl;
                    std::cout<<"Tentative best case id: ... "<<mpso_id_tentative_best_case_.value(ObjFn_id)->GetId().toString().toStdString()<<std::endl;
                }
            }
        }

        Optimizer::TerminationCondition MPSO::IsFinished() {
            if (case_handler_->CasesBeingEvaluated().size() > 0) return NOT_FINISHED;
            if (iteration_ < max_iterations_) return NOT_FINISHED;
            else return MAX_EVALS_REACHED;
        }

        Case *MPSO::generateRandomCase(const QUuid &ObjFn_id) {
            Case *new_case;

            Case *tentative_best_case = mpso_id_tentative_best_case_.value(ObjFn_id);
            new_case = new Case(tentative_best_case);

            new_case->set_mpso_id_r_CO2(mpso_id_r_CO2_);
            new_case->set_mpso_id_ofv(tentative_best_case->mpso_id_ofv());
            new_case->set_ObjFn_id(ObjFn_id);

            Eigen::VectorXd erands(n_vars_);
            for (int i = 0; i < n_vars_; ++i) {
                erands(i) = random_doubles(gen_, lower_bound_(i), upper_bound_(i), 1)[0];
            }
            new_case->SetRealVarValues(erands);

            return  new_case;
        }

        MPSO::Particle::Particle(Optimization::Case *c, boost::random::mt19937 &gen, VectorXd v_max, int n_vars) {
            case_pointer = c;
            rea_vars=c->GetRealVarVector();
            Eigen::VectorXd temp(n_vars);
            for(int i = 0; i < rea_vars.size(); i++){
                temp(i) = random_doubles(gen, -v_max(i), v_max(i), 1)[0];
            }
            rea_vars_velocity=temp;
        }

        QHash<QUuid, MPSO::Particle> MPSO::get_mpso_id_current_global_best_particle(){
            QHash<QUuid, Particle> mpso_id_current_global_best_particle = QHash<QUuid, Particle> ();
            QList<QUuid> ObjFn_ids = mpso_id_r_CO2_.keys();
            for (int i = 0; i < mpso_nr_of_swarms_; ++i) {
                QUuid ObjFn_id = ObjFn_ids[i];

                //! Initialize the current global best particle (corresponds to an objective function id)
                Particle current_global_best_particle;
                if(mpso_id_swarm_memory_.value(ObjFn_id).size() == 1){
                    current_global_best_particle = mpso_id_swarm_memory_.value(ObjFn_id)[0][0];
                }
                else {
                    current_global_best_particle = mpso_id_current_global_best_particle_.value(ObjFn_id);
                }

                //! Update the current global best particle
                for (int j = 0; j < mpso_nr_of_swarms_; ++j){
                    vector<vector<Particle>> swarm_memory = mpso_id_swarm_memory_.value(ObjFn_ids[j]);
                    for (int k = 0; k < swarm_memory.size(); ++k){
                        vector<Particle> swarm = swarm_memory[k];
                        for (int l = 0; l < swarm.size(); ++l) {
                            double particle_ofv = swarm[l].case_pointer->mpso_id_ofv().value(ObjFn_id);
                            double current_global_best_particle_ofv = current_global_best_particle.case_pointer->mpso_id_ofv().value(ObjFn_id);
                            if (mode_ == Settings::Optimizer::OptimizerMode::Maximize) {
                                if (particle_ofv > current_global_best_particle_ofv) {
                                    current_global_best_particle = swarm[l];
                                }
                            }
                            else if (mode_ == Settings::Optimizer::OptimizerMode::Minimize) {
                                if (particle_ofv < current_global_best_particle_ofv) {
                                    current_global_best_particle = swarm[l];
                                }
                            }
                        }
                    }
                }

                mpso_id_current_global_best_particle.insert(ObjFn_id, current_global_best_particle);
            }

            return mpso_id_current_global_best_particle;
        }

        MPSO::Particle MPSO::find_best_in_particle_memory(const QUuid &ObjFn_id, int particle_num){
            vector<vector<Particle>> swarm_memory = mpso_id_swarm_memory_.value(ObjFn_id);
            Particle best_in_particle_memory = swarm_memory[0][particle_num];
            for (int i = 1; i < swarm_memory.size(); ++i) {
                double particle_ofv = swarm_memory[i][particle_num].case_pointer->mpso_id_ofv().value(ObjFn_id);
                double best_particle_ofv = best_in_particle_memory.case_pointer->mpso_id_ofv().value(ObjFn_id);
                if (mode_ == Settings::Optimizer::OptimizerMode::Maximize) {
                    if (particle_ofv > best_particle_ofv) {
                        best_in_particle_memory = swarm_memory[i][particle_num];
                    }
                }
                else if (mode_ == Settings::Optimizer::OptimizerMode::Minimize) {
                    if (particle_ofv < best_particle_ofv) {
                        best_in_particle_memory = swarm_memory[i][particle_num];
                    }
                }
            }
            return best_in_particle_memory;
        }

        vector<MPSO::Particle> MPSO::update_velocity(const QUuid &ObjFn_id) {
            vector<Particle> swarm = mpso_id_swarm_.value(ObjFn_id);
            vector<Particle> new_swarm = vector<Particle> ();
            for (int i = 0; i < number_of_particles_; ++i) {
                Particle best_in_particle_memory = find_best_in_particle_memory(ObjFn_id, i);
                new_swarm.push_back(swarm[i]);
                for(int j = 0; j < n_vars_; ++j){
                    double velocity_1 = learning_factor_1_ * random_double(gen_, 0, 1) * (best_in_particle_memory.rea_vars(j)-swarm[i].rea_vars(j));
                    double velocity_2 = learning_factor_2_ * random_double(gen_, 0, 1) * (mpso_id_current_global_best_particle_.value(ObjFn_id).rea_vars(j)-swarm[i].rea_vars(j));
                    new_swarm[i].rea_vars_velocity(j) = swarm[i].rea_vars_velocity(j) + velocity_1 + velocity_2;
                    if (new_swarm[i].rea_vars_velocity(j) < -v_max_(j)){
                        new_swarm[i].rea_vars_velocity(j) = -v_max_(j);
                    }
                    else if(new_swarm[i].rea_vars_velocity(j) > v_max_(j)){
                        new_swarm[i].rea_vars_velocity(j) = v_max_(j);
                    }
                }
            }
            return new_swarm;
        }

        vector<MPSO::Particle> MPSO::update_position(const QUuid &ObjFn_id) {
            vector<Particle> swarm = mpso_id_swarm_.value(ObjFn_id);
            for(int i = 0; i < number_of_particles_; ++i){
                for(int j = 0; j < n_vars_; ++j){
                    swarm[i].rea_vars(j)=swarm[i].rea_vars_velocity(j)+swarm[i].rea_vars(j);
                    if (swarm[i].rea_vars(j) > upper_bound_[j]){
                        swarm[i].rea_vars(j) = upper_bound_[j]-abs(swarm[i].rea_vars(j)-upper_bound_[j])*0.5;
                        swarm[i].rea_vars_velocity(j) = swarm[i].rea_vars_velocity(j)*-0.5;
                    }
                    else if (swarm[i].rea_vars(j) < lower_bound_[j]){
                        swarm[i].rea_vars(j) = lower_bound_[j]+abs(swarm[i].rea_vars(j)-lower_bound_[j])*0.5;
                        swarm[i].rea_vars_velocity(j) = swarm[i].rea_vars_velocity(j)*-0.5;
                    }
                }
            }
            return swarm;
        }

        void MPSO::Particle::ParticleAdapt(Eigen::VectorXd rea_vars_velocity_swap, Eigen::VectorXd rea_vars_swap){
            rea_vars=rea_vars_swap;
            case_pointer->SetRealVarValues(rea_vars);
            rea_vars_velocity = rea_vars_velocity_swap;
            case_pointer->SetRealVarVelocity(rea_vars_velocity);
        }

        bool MPSO::isImprovement(const Case *c, const QUuid &ObjFn_id) {
            if (mode_ == Settings::Optimizer::OptimizerMode::Maximize) {
                if (c->mpso_id_ofv().value(ObjFn_id) > mpso_id_tentative_best_case_.value(ObjFn_id)->mpso_id_ofv().value(ObjFn_id)) {
                    return true;
                }
            }
            else if (mode_ == Settings::Optimizer::OptimizerMode::Minimize) {
                if (c->mpso_id_ofv().value(ObjFn_id) < mpso_id_tentative_best_case_.value(ObjFn_id)->mpso_id_ofv().value(ObjFn_id)) {
                    return true;
                }
            }
            return false;
        }

        void MPSO::updateTentativeBestCase(Case *c, const QUuid &ObjFn_id) {
            mpso_id_tentative_best_case_.insert(ObjFn_id, c);
            mpso_id_tentative_best_case_iteration_.insert(ObjFn_id, iteration_);
        }
    }
}
