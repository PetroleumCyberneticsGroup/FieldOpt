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

#include "BO_PSO.h"

#include <Utilities/verbosity.h>
#include "Utilities/printer.hpp"
#include "Utilities/stringhelpers.hpp"
#include "gp/rprop.h"
#include "Utilities/math.hpp"
#include "Utilities/random.hpp"
#include "Utilities/time.hpp"
#include "optimizers/bayesian_optimization/af_optimizers/AFPSO.h"
#include "BO_PSO.h"

namespace Optimization {
namespace Optimizers {
namespace BayesianOptimization {
    BO_PSO::BO_PSO(Settings::Optimizer *settings,
             Case *base_case,
             Model::Properties::VariablePropertyContainer *variables,
             Reservoir::Grid::Grid *grid,
             Logger *logger,
             CaseHandler *case_handler,
             Constraints::ConstraintHandler *constraint_handler
    ) : Optimizer(settings, base_case, variables, grid, logger, case_handler, constraint_handler) {

        time_fitting_ = 0;
        time_af_opt_ = 0;
        settings_ = settings;
        gen_ = get_random_generator(settings->parameters().rng_seed);
        int n_cont_vars = variables->ContinousVariableSize();
        n_vars_ = variables->ContinousVariableSize();
        max_iterations_ = settings->parameters().max_generations;
        stagnation_limit_ = settings->parameters().stagnation_limit;
        learning_factor_1_ = settings->parameters().pso_learning_factor_1;
        learning_factor_2_ = settings->parameters().pso_learning_factor_2;
        number_of_particles_ = settings->parameters().pso_swarm_size;

        if (constraint_handler_->HasBoundaryConstraints()) {
            lb_ = constraint_handler_->GetLowerBounds(base_case->GetRealVarIdVector());
            ub_ = constraint_handler_->GetUpperBounds(base_case->GetRealVarIdVector());
        }
        else {
            lb_.resize(base_case->GetRealVarIdVector().size());
            ub_.resize(base_case->GetRealVarIdVector().size());
            lb_.fill(settings->parameters().lower_bound);
            ub_.fill(settings->parameters().upper_bound);
        }

        auto difference = ub_ - lb_;
        v_max_ = difference * settings->parameters().pso_velocity_scale;
        for (int i = 0; i < number_of_particles_; ++i) {
            auto new_case = generateRandomCase();
            swarm_.push_back(Particle(new_case ,gen_, v_max_, n_vars_));
            case_handler_->AddNewCase(new_case);
        }

        af_ = AcquisitionFunction(settings->parameters());
        af_opt_ = AFOptimizers::AFPSO(lb_, ub_, settings->parameters().rng_seed);
        gp_ = new libgp::GaussianProcess(n_cont_vars, settings->parameters().ego_kernel);


        if (settings->parameters().ego_init_sampling_method == "Random") {
            if (settings->parameters().ego_init_guesses == -1) {
                n_initial_guesses_ = n_cont_vars * 2;
            }
            else {
                n_initial_guesses_ = settings->parameters().ego_init_guesses;
            }
            auto rng = get_random_generator(settings->parameters().rng_seed);
            for (int i = 0; i < n_initial_guesses_; ++i) {
                VectorXd pos = VectorXd::Zero(lb_.size());
                for (int i = 0; i < lb_.size(); ++i) {
                    pos(i) = random_double(rng, lb_(i), ub_(i));
                }
                Case * init_case = new Case(base_case);
                init_case->SetRealVarValues(pos);
                case_handler_->AddNewCase(init_case);
            }
        }
        else if (settings->parameters().ego_init_sampling_method == "Uniform") {
            // Step half-way from initial guess to bounds on each axis
            // and all the way to bounds.
            auto init_values = base_case->GetRealVarVector();
            for (int i = 0; i < init_values.size(); ++i) {
                vector<double> values;
                values.push_back(init_values[i] + abs(ub_[i] - init_values[i]) / 2.0);
                values.push_back(init_values[i] - abs(init_values[i] - lb_[i]) / 2.0);
                values.push_back(ub_[i]);
                values.push_back(lb_[i]);

                for (double value : values) {
                    Case * pert_case = new Case(base_case);
                    auto pert_vec = init_values;
                    pert_vec[i] = value;
                    pert_case->SetRealVarValues(pert_vec);
                    case_handler_->AddNewCase(pert_case);
                }
            }
        }
        else {
            Printer::ext_warn("Only the Random and Uniform sampling methods are implemented.", "Optimization", "EGO");
            throw std::runtime_error("Failed to initialize EGO optimizer.");
        }

        // Initialize GP hyperparameters
        std::map<std::string, int> map_kernel_to_n_hyper = {
                { "CovLinearard",          n_cont_vars   },
                { "CovLinearone",          1             },
                { "CovMatern3iso",         2             },
                { "CovMatern5iso",         2             },
                { "CovNoise",              1             },
                { "CovRQiso",              3             },
                { "CovSEard",              n_cont_vars+1 },
                { "CovSEiso",              2             },
                { "CovPeriodicMatern3iso", 3             },
                { "CovPeriodic",           3             }
        };
        Eigen::VectorXd params(map_kernel_to_n_hyper[settings->parameters().ego_kernel]);
        params.fill(-1);
        gp_->covf().set_loghyper(params);


        if (enable_logging_) {
            logger_->AddEntry(new ConfigurationSummary(this));
        }
    }

    Case *BO_PSO::generateRandomCase() {
        Case *new_case;
        new_case = new Case(GetTentativeBestCase());

        Eigen::VectorXd erands(n_vars_);
        for (int i = 0; i < n_vars_; ++i) {
            erands(i) = random_doubles(gen_, lb_(i), ub_(i), 1)[0];
        }
        new_case->SetRealVarValues(erands);
        return  new_case;
    }

    BO_PSO::Particle::Particle(Optimization::Case *c, boost::random::mt19937 &gen, VectorXd v_max, int n_vars) {
        case_pointer = c;
        rea_vars=c->GetRealVarVector();
        Eigen::VectorXd temp(n_vars);
        for(int i = 0; i < rea_vars.size(); i++){
            temp(i) = random_doubles(gen, -v_max(i), v_max(i), 1)[0];
        }
        rea_vars_velocity=temp;
    }

    void BO_PSO::Particle::ParticleAdapt(Eigen::VectorXd rea_vars_velocity_swap, Eigen::VectorXd rea_vars_swap){
        rea_vars=rea_vars_swap;
        case_pointer->SetRealVarValues(rea_vars);
        rea_vars_velocity = rea_vars_velocity_swap;
    }

    bool BO_PSO::is_stagnant() {
        vector<double> list_of_sums;
        for (auto chrom : swarm_) {
            list_of_sums.push_back(chrom.rea_vars.sum());
        }
        double stdev = calc_standard_deviation(list_of_sums);
        return stdev <= stagnation_limit_;
    }

    BO_PSO::Particle BO_PSO::get_global_best(){
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
        if(best_particle.case_pointer->objective_function_value() < tentative_best_case_->objective_function_value()){
            best_particle.rea_vars = tentative_best_case_->GetRealVarVector();
            cout << "EGO candidate is so far better" << endl;
        }
        return best_particle;
    }

    BO_PSO::Particle BO_PSO::find_best_in_particle_memory(int particle_num){
        Particle best_in_particle_memory = swarm_memory_[0][particle_num];
        for(int i = 1; i < swarm_memory_.size(); i++) {
            if (isBetter(swarm_memory_[i][particle_num].case_pointer, best_in_particle_memory.case_pointer)) {
                best_in_particle_memory = swarm_memory_[i][particle_num];
            }
        }
        return best_in_particle_memory;
    }

    vector<BO_PSO::Particle> BO_PSO::update_velocity() {
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

    vector<BO_PSO::Particle> BO_PSO::update_position() {
        for(int i = 0; i < swarm_.size(); i++){
            for(int j = 0; j < n_vars_; j++){
                swarm_[i].rea_vars(j)=swarm_[i].rea_vars_velocity(j)+swarm_[i].rea_vars(j);
                if (swarm_[i].rea_vars(j) > ub_[j]){
                    swarm_[i].rea_vars(j) = ub_[j]-abs(swarm_[i].rea_vars(j)-ub_[j])*0.5;
                    swarm_[i].rea_vars_velocity(j) = swarm_[i].rea_vars_velocity(j)*-0.5;
                } else if (swarm_[i].rea_vars(j) < lb_[j]){
                    swarm_[i].rea_vars(j) = lb_[j]+abs(swarm_[i].rea_vars(j)-lb_[j])*0.5;
                    swarm_[i].rea_vars_velocity(j) = swarm_[i].rea_vars_velocity(j)*-0.5;
                }
            }
        }
        return swarm_;
    }

    Optimization::Optimizer::TerminationCondition BO_PSO::IsFinished() {
        TerminationCondition tc = NOT_FINISHED;
        if (case_handler_->CasesBeingEvaluated().size() > 0)
            return tc;
        if (iteration_ > max_iterations_)
            tc = MAX_EVALS_REACHED;
        if (tc != NOT_FINISHED) {
            map<string, string> ext_state;
            ext_state["Time in AF opt"] = boost::lexical_cast<string>(time_af_opt_);
            ext_state["Time in GP opt"] = boost::lexical_cast<string>(time_fitting_);
            if (enable_logging_) {
                logger_->AddEntry(this);
                logger_->AddEntry(new Summary(this, tc, ext_state));
            }
        }
        return tc;
    }
    void BO_PSO::handleEvaluatedCase(Case *c) {
        if (!normalizer_ofv_.is_ready())
            initializeNormalizers();

        gp_->add_pattern(c->GetRealVarVector().data(), normalizer_ofv_.normalize(c->objective_function_value()));
        if (isImprovement(c)) {
            updateTentativeBestCase(c);
            Printer::ext_info("Found new tentative best case: " + Printer::num2str(c->objective_function_value()), "Optimization", "EGO");
        }
    }
    void BO_PSO::iterate() {
        if (!normalizer_ofv_.is_ready()) {
            initializeNormalizers();
            // Add base case to GP
            gp_->add_pattern(tentative_best_case_->GetRealVarVector().data(), normalizer_ofv_.normalize(tentative_best_case_->objective_function_value()));
        }

        // Optimize GP hyperparameters
        QDateTime start, end;
        start = QDateTime::currentDateTime();
        libgp::RProp rprop;
        rprop.init();
        if (VERB_OPT >= 3) {
            Printer::ext_info("Optimizing Gaussian Process kernel hyperparameters ... ", "Optimization", "EGO");
            rprop.maximize(gp_, 100, 1);
        }
        else {
            rprop.maximize(gp_, 100, 0);
        }
        end = QDateTime::currentDateTime();
        time_fitting_ += time_span_seconds(start, end);
        cout << time_span_seconds(start,end) << endl;

        start = QDateTime::currentDateTime();
        VectorXd new_position = af_opt_.Optimize(
                gp_, af_,
                normalizer_ofv_.normalize(GetTentativeBestCase()->objective_function_value())
        );
        end = QDateTime::currentDateTime();
        time_af_opt_ += time_span_seconds(start, end);

        for (int i = 0; i < new_position.size(); ++i) {
            if (new_position(i) < lb_(i)) {
                new_position(i) = lb_(i);
                cout << "Snapped to LB." << endl;
            } else if (new_position(i) > ub_(i)) {
                new_position(i) = ub_(i);
                cout << "Snapped to UB." << endl;
            }
        }
        Case *new_case = new Case(case_handler_->AllCases()[0]);
        new_case->SetRealVarValues(new_position);
        case_handler_->AddNewCase(new_case);
        if (enable_logging_) {
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
        cout << "Generation: " << iteration_ << endl;
        iteration_++;
    }

    Loggable::LogTarget BO_PSO::ConfigurationSummary::GetLogTarget() {
        return LOG_SUMMARY;
    }
    map<string, string> BO_PSO::ConfigurationSummary::GetState() {
        map<string, string> statemap;
        statemap["Name"] = "Efficient Global Optimization (EGO)";
        statemap["Kernel"] = opt_->settings_->parameters().ego_kernel;
        statemap["Acquisition function"] = opt_->settings_->parameters().ego_af;
        statemap["AF Optimizer"] = "PSO";
        statemap["Mode"] = opt_->mode_ == Settings::Optimizer::OptimizerMode::Maximize ? "Maximize" : "Minimize";
        statemap["Max Evaluations"] = boost::lexical_cast<string>(opt_->max_evaluations_);
        statemap["Num. initial guesses"] = boost::lexical_cast<string>(opt_->n_initial_guesses_);

        string constraints_used = "";
        for (auto cons : opt_->constraint_handler_->constraints()) {
            constraints_used += cons->name() + " ";
        }
        statemap["Constraints"] = constraints_used;
        return statemap;
    }
    QUuid BO_PSO::ConfigurationSummary::GetId() {
        return QUuid(); // Null UUID
    }
    map<string, vector<double>> BO_PSO::ConfigurationSummary::GetValues() {
        map<string, vector<double>> valmap;
        return valmap;
    }

}
}
}