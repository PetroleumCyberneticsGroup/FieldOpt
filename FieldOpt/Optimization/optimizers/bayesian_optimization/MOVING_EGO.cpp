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

#include <Utilities/verbosity.h>
#include "Utilities/printer.hpp"
#include "Utilities/stringhelpers.hpp"
#include "gp/rprop.h"
#include "Utilities/math.hpp"
#include <math.h>
#include "Utilities/random.hpp"
#include "Utilities/time.hpp"
#include "optimizers/bayesian_optimization/af_optimizers/AFPSO.h"
#include "MOVING_EGO.h"

namespace Optimization {
    namespace Optimizers {
        namespace BayesianOptimization {
            MOVING_EGO::MOVING_EGO(Settings::Optimizer *settings,
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
                int n_cont_vars = variables->ContinousVariableSize();
                n_vars_ = variables->ContinousVariableSize();
                max_iterations_ = 20;//settings->parameters().max_generations;

                if (constraint_handler_->HasBoundaryConstraints()) {
                    lb_ = constraint_handler_->GetLowerBounds(base_case->GetRealVarIdVector());
                    ub_ = constraint_handler_->GetUpperBounds(base_case->GetRealVarIdVector());
                } else {
                    lb_.resize(base_case->GetRealVarIdVector().size());
                    ub_.resize(base_case->GetRealVarIdVector().size());
                    lb_.fill(settings->parameters().lower_bound);
                    ub_.fill(settings->parameters().upper_bound);
                }

                af_ = AcquisitionFunction(settings->parameters());
                //af_opt_ = AFOptimizers::AFPSO(lb_, ub_, settings->parameters().rng_seed);
                gp_ = new libgp::GaussianProcess(n_vars_, settings->parameters().ego_kernel);
                xmean_ = Eigen::VectorXd::Zero(n_vars_);
                Eigen::VectorXd base_case_variables = base_case->GetRealVarVector();
                improve_base_case_ = true;
                if(improve_base_case_ == true){
                    for (int i = 0; i < n_vars_; i++){
                        xmean_(i) = (base_case_variables(i)-lb_(i))/(ub_(i)-lb_(i));
                    }
                } else{
                    for(int i = 0; i < n_vars_; i++){
                        xmean_(i) = random_doubles(gen_, 0, 1, 1)[0];
                    }
                }

                settings_->parameters().ego_init_sampling_method = "Normal";

                // Initialize GP hyperparameters
                std::map<std::string, int> map_kernel_to_n_hyper = {
                        {"CovLinearard",          n_cont_vars},
                        {"CovLinearone",          1},
                        {"CovMatern3iso",         2},
                        {"CovMatern5iso",         2},
                        {"CovNoise",              1},
                        {"CovRQiso",              3},
                        {"CovSEard",              n_cont_vars + 1},
                        {"CovSEiso",              2},
                        {"CovPeriodicMatern3iso", 3},
                        {"CovPeriodic",           3}
                };
                Eigen::VectorXd params(map_kernel_to_n_hyper[settings->parameters().ego_kernel]);
                params.fill(-1);
                gp_->covf().set_loghyper(params);


                if (enable_logging_) {
                    logger_->AddEntry(new ConfigurationSummary(this));
                }


                //Strategy parameter setting: Selection
                lambda_ = 25; //4+floor(3*log(n_vars_));
                mu_ = lambda_/2;
                for(int i = 1; i <= mu_; i++){
                    weights_.push_back(log(mu_+0.5)-log(i));
                }
                mu_ = floor(mu_);
                double sum_of_weights = 0;
                for(int i = 0; i < weights_.size(); i++){
                    sum_of_weights += weights_[i];
                }

                for(int i = 0; i < weights_.size(); i++){
                    weights_[i] = weights_[i]/sum_of_weights;
                }
                double temp_sum_of_weights = 0;
                double temp_sum_of_variance_of_weights = 0;
                for(int i = 0; i < weights_.size(); i++){
                    temp_sum_of_weights += weights_[i];
                    temp_sum_of_variance_of_weights += pow(weights_[i], 2);

                }
                mueff_ = temp_sum_of_weights/temp_sum_of_variance_of_weights;

                //Strategy parameter setting: Adaptation
                cc_ = (4+mueff_/n_vars_) / (n_vars_+4 + 2*mueff_/n_vars_);
                cs_ = (mueff_+2) / (n_vars_+mueff_+5);
                c1_ = 2 / (pow(n_vars_+1.3,2)+mueff_);

                if(1-c1_ < 2 * (mueff_-2+1/mueff_) / (pow(n_vars_+2,2)+mueff_)){
                    cmu_ = 1-c1_;
                }else{
                    cmu_ = 2 * (mueff_-2+1/mueff_) / (pow(n_vars_+2,2)+mueff_);
                }

                if(0 > sqrt((mueff_-1)/(n_vars_+1))-1){
                    damps_ = 1 + 2 * 0 + cs_;
                }else{
                    damps_ = 1 + 2 * sqrt((mueff_-1)/(n_vars_+1))-1 + cs_;
                }

                // Initialize dynamic (internal) strategy parameters and constants

                pc_ = Eigen::VectorXd::Zero(n_vars_);
                ps_ = Eigen::VectorXd::Zero(n_vars_);
                B_ = Eigen::MatrixXd::Identity(n_vars_, n_vars_);
                D_ = Eigen::VectorXd::Ones(n_vars_);
                Eigen::VectorXd temp_D = Eigen::VectorXd::Zero(n_vars_);
                for(int i = 0; i < D_.size(); i++){
                    temp_D[i] = D_(i)*D_(i);
                }

                C_ = B_ * (temp_D).asDiagonal() * B_.transpose();

                for(int i = 0; i < D_.size(); i++){
                    temp_D[i] = 1.0/D_(i);
                }
                invsqrtC_ = B_ * (temp_D).asDiagonal() * B_.transpose();
                eigeneval_ = 0;
                chiN_ = pow(n_vars_, 0.5) * (1-(float(1)/(4*n_vars_))+1/(21*pow(n_vars_,2)));
                sigma_ = 0.3;
                radius_ = 0.05;

                for (int i = 0; i < lambda_; ++i) {
                    auto new_case = generateCase(xmean_, i, 1);
                    case_handler_->AddNewCase(new_case);
                }

            }

            MOVING_EGO::Individual::Individual(Optimization::Case *c, boost::random::mt19937 &gen, int index, Eigen::VectorXd erands_norm, double penalty_dist) {
                case_pointer_ = c;
                rea_vars_=c->GetRealVarVector();
                erands_norm_ = erands_norm;
                index_ = index;
                penalty_dist_ = penalty_dist;
            }


            Case *MOVING_EGO::generateCase(Eigen::VectorXd xmean, int index, bool first_iteration){
                Case *new_case;
                new_case = new Case(GetTentativeBestCase());
                Eigen::VectorXd xmeanNormal(n_vars_);
                Eigen::VectorXd erands_norm(n_vars_);
                Eigen::VectorXd erands(n_vars_);
                double penalty_dist = 0;
                for (int i = 0; i < n_vars_; ++i){
                    xmeanNormal(i) = d(gen);
                }
                for (int i = 0; i < n_vars_; ++i) {
                    erands_norm(i) = xmean(i) + radius_ * B_(i,i) * (D_(i) * xmeanNormal(i)); //sigma_ * B_(i,i) * (D_(i) * xmeanNormal(i));
                    if (erands_norm(i) > 1.0){
                        penalty_dist += abs(erands_norm(i)-1.0);
                    } else if (erands_norm(i) < 0.0){
                        penalty_dist += abs(erands_norm(i));
                    } else {
                        penalty_dist += 0;
                    }
                    erands(i) = lb_(i)+erands_norm(i)*(ub_(i)-lb_(i));
                    if (erands(i) < lb_(i)){
                        erands(i) = lb_(i);
                    }else if ( erands(i) > ub_(i)){
                        erands(i) = ub_(i);
                    }
                }

                const IOFormat fmt(4, DontAlignCols, "\t", " ", "\n", "", "", "");
                //cout << erands_norm.format(fmt) << endl;
                //cout << xmean.format(fmt) << endl;
                //cout << D_.format(fmt) << endl;
                new_case->SetRealVarValues(erands);
                if (first_iteration){
                    population_.push_back(Individual(new_case, gen_, index, erands_norm, penalty_dist));
                } else {
                    temp_population_.push_back(Individual(new_case, gen_, index, erands_norm, penalty_dist));
                }
                return new_case;
            }

            Case *MOVING_EGO::generateSpecificCase(Eigen::VectorXd xmean, int index, bool first_iteration){
                Case *new_case;
                new_case = new Case(GetTentativeBestCase());
                Eigen::VectorXd xmeanNormal(n_vars_);
                Eigen::VectorXd erands_norm(n_vars_);
                Eigen::VectorXd erands(n_vars_);
                double penalty_dist = 0;

                for (int i = 0; i < n_vars_; ++i) {
                    erands_norm(i) = xmean(i);
                    erands(i) = lb_(i)+erands_norm(i)*(ub_(i)-lb_(i));
                    if (erands(i) < lb_(i)){
                        erands(i) = lb_(i);
                    }else if ( erands(i) > ub_(i)){
                        erands(i) = ub_(i);
                    }
                }

                new_case->SetRealVarValues(erands);
                return new_case;
            }

            Optimization::Optimizer::TerminationCondition MOVING_EGO::IsFinished() {
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

            void MOVING_EGO::handleEvaluatedCase(Case *c) {
                if (!normalizer_ofv_.is_ready())
                    initializeNormalizers();

                auto case_variables = c->GetRealVarVector();
                Eigen::VectorXd gp_variables = Eigen::VectorXd::Zero(n_vars_);
                for(int i = 0; i < n_vars_; i++){
                    gp_variables(i) = (case_variables(i)-lb_(i))/(ub_(i)-lb_(i));
                }

                gp_->add_pattern(gp_variables.data(),
                                 normalizer_ofv_.normalize(c->objective_function_value()));

                /*Printer::ext_info(
                        "Current objective function value " + Printer::num2str(c->objective_function_value()),
                        "Optimization", "MOVING_EGO");*/


                if (isImprovement(c)) {
                    updateTentativeBestCase(c);
                    Printer::ext_info(
                            "Found new tentative best case: " + Printer::num2str(c->objective_function_value()) + " In iteration " + Printer::num2str(iteration_)
                            + " Number of evaluated cases: " + Printer::num2str(case_handler_->EvaluatedCases().size()),
                            "Optimization", "EGO");
                }
            }

            vector<MOVING_EGO::Individual> MOVING_EGO::sortPopulation(vector<MOVING_EGO::Individual> population){
                vector<MOVING_EGO::Individual> sortedPopulation;
                int n = population.size();
                bool swapped = true;
                while(swapped) {
                    swapped = false;
                    for (int i = 1; i <= n - 1; i++) {
                        if (population[i - 1].ofv() > population[i].ofv() && (Settings::Optimizer::Minimize == settings_->mode())) {
                            MOVING_EGO::Individual temp_individual = population[i];
                            population[i] = population[i - 1];
                            population[i - 1] = temp_individual;
                            swapped = true;
                        } else if (population[i - 1].ofv() < population[i].ofv() && (Settings::Optimizer::Maximize == settings_->mode())) {
                            MOVING_EGO::Individual temp_individual = population[i];
                            population[i] = population[i - 1];
                            population[i - 1] = temp_individual;
                            swapped = true;
                        }
                    }
                }
                return population;
            }

            void MOVING_EGO::iterate() {
                if (!normalizer_ofv_.is_ready()) {
                    initializeNormalizers();
                    // Add base case to GP
                    auto case_variables = tentative_best_case_->GetRealVarVector();
                    Eigen::VectorXd gp_variables = Eigen::VectorXd::Zero(n_vars_);
                    for(int i = 0; i < n_vars_; i++){
                        gp_variables(i) = (case_variables(i)-lb_(i))/(ub_(i)-lb_(i));
                    }

                    gp_->add_pattern(gp_variables.data(),
                                     normalizer_ofv_.normalize(tentative_best_case_->objective_function_value()));
                }
                const IOFormat fmt(2, DontAlignCols, "\t", " ", "", "", "", "");
                for( int i = 0; i < population_.size(); i++ ){
                    if(Settings::Optimizer::Minimize == settings_->mode()){
                        population_[i].case_pointer_->set_objective_function_value(population_[i].ofv()+(exp(penalty_*population_[i].penalty_dist_)-1));
                    } else {
                        population_[i].case_pointer_->set_objective_function_value(population_[i].ofv()-(exp(penalty_*population_[i].penalty_dist_)-1));
                    }
                    //    cout << "Adjusted individual ofv: " << population_[i].ofv() << endl;
                }


                // Find clb_ and cub_
                clb_ = Eigen::VectorXd::Zero(n_vars_);
                cub_ = Eigen::VectorXd::Zero(n_vars_);

                /*for (int i = 0; i < population_.size(); i++){
                    auto boundary_variables = population_[i].case_pointer_->GetRealVarVector();
                    auto boundary_variables_norm = population_[i].erands_norm_;
                    for (int j = 0; j < n_vars_; j++){
                        double real_xmean = lb_(j)+xmean_(j)*(ub_(j)-lb_(j));
                        if (cub_(j) < boundary_variables_norm[j] || clb_(j) > boundary_variables_norm[j]){
                            double diff;
                            if(cub_(j) < boundary_variables_norm[j]){
                                diff = abs(xmean_(j) - boundary_variables_norm[j]);
                            } else {
                                diff = abs(boundary_variables_norm[j] - xmean_(j));
                            }
                            //cout << "Difference: " <<  diff << endl;
                            cub_(j) = xmean_(j)+diff;
                            clb_(j) = xmean_(j)-diff;
                            //cout << "Upper limit: " << cub_(j) << " variable value: " << (boundary_variables[j]-lb_(j))/(ub_(j)-lb_(j)) << " lower limit: " << clb_(j) << endl;
                            //cout << "real_xmean: " << real_xmean << " boundary_variables: " << boundary_variables[j] << endl;
                        }
                    }
                }*/

                for (int j = 0; j < n_vars_; j++){
                    //cout <<  "UpperLimit: " << xmean_(j)+abs(xmean_(j)*radius_) << " xMEAN: " << xmean_(j) << endl;
                    cub_(j) = xmean_(j)+abs(xmean_(j)*radius_);
                    clb_(j) = xmean_(j)-abs(xmean_(j)*radius_);
                }

                for(int i = 0; i < cub_.size(); i++){
                    //cout << "This is the cub_: " << cub_(i) << " This is the clb_: " << clb_(i) << endl;
                }

                /*double n_extra = 0;
                for(int i = 0; i < population_cache_.size(); i++){
                    bool within = true;
                    auto boundary_variables_norm = population_cache_[i].erands_norm_;
                    for(int j = 0; j < n_vars_; j++){
                        //cout << "Upper limit: " << cub_(j) << " variable value: " << boundary_variables[j] << " lower limit: " << clb_(j) << endl;
                        if ((cub_(j)) < boundary_variables_norm[j] || (clb_(j)) > boundary_variables_norm[j]){
                            within = false;
                        }
                    }
                    if (within) {
                        Eigen::VectorXd gp_variables = Eigen::VectorXd::Zero(n_vars_);
                        for(int k = 0; k < n_vars_; k++){
                            gp_variables(k) = boundary_variables_norm(k);
                        }
                        gp_->add_pattern(gp_variables.data(),
                                         normalizer_ofv_.normalize(population_cache_[i].case_pointer_->objective_function_value()));
                        n_extra += 1;
                    }

                }
                cout << "This is how many we got extra: " << n_extra << endl;*/
                af_opt_ = AFOptimizers::AFPSO(clb_, cub_, settings_->parameters().rng_seed);

                population_ = sortPopulation(population_);
                xold_ = xmean_;

                // Optimize GP hyperparameters
                QDateTime start, end;
                start = QDateTime::currentDateTime();
                libgp::RProp rprop;
                rprop.init();
                if (VERB_OPT >= 3) {
                    Printer::ext_info("Optimizing Gaussian Process kernel hyperparameters ... ", "Optimization", "EGO");
                    rprop.maximize(gp_, 100, 1);
                } else {
                    rprop.maximize(gp_, 100, 0);
                }
                end = QDateTime::currentDateTime();

                time_fitting_ += time_span_seconds(start, end);
                //cout << time_span_seconds(start, end) << endl;

                start = QDateTime::currentDateTime();
                VectorXd xmean_ = af_opt_.Optimize(
                        gp_, af_,
                        normalizer_ofv_.normalize(GetTentativeBestCase()->objective_function_value())
                );

                end = QDateTime::currentDateTime();
                time_af_opt_ += time_span_seconds(start, end);


                updateEvolutionPath();
                adaptCovarianceMatrix();
                decompositionOfC();


                gp_ = new libgp::GaussianProcess(n_vars_, settings_->parameters().ego_kernel);

                // Initialize GP hyperparameters
                std::map<std::string, int> map_kernel_to_n_hyper = {
                        {"CovLinearard",          n_vars_},
                        {"CovLinearone",          1},
                        {"CovMatern3iso",         2},
                        {"CovMatern5iso",         2},
                        {"CovNoise",              1},
                        {"CovRQiso",              3},
                        {"CovSEard",              n_vars_ + 1},
                        {"CovSEiso",              2},
                        {"CovPeriodicMatern3iso", 3},
                        {"CovPeriodic",           3}
                };
                Eigen::VectorXd params(map_kernel_to_n_hyper[settings_->parameters().ego_kernel]);
                params.fill(-1);
                gp_->covf().set_loghyper(params);

                auto new_case = generateSpecificCase(xmean_, 1, 0);
                case_handler_->AddNewCase(new_case);

                for (int i = 0; i < lambda_; ++i) {
                    auto new_case = generateCase(xmean_, i, 0);
                    case_handler_->AddNewCase(new_case);
                }


                //cout << xmean_.format(fmt) << endl;

                if (enable_logging_) {
                    logger_->AddEntry(this);
                }
                //cout << "Evaluated cases: " << case_handler_->EvaluatedCases().size() << endl;
                for (int i = 0; i < population_.size(); i++){
                    population_cache_.emplace_back(population_[i]);
                }
                population_ = temp_population_;
                //cout << xmean_.format(fmt) << endl;
                iteration_++;
            }

            Loggable::LogTarget MOVING_EGO::ConfigurationSummary::GetLogTarget() {
                return LOG_SUMMARY;
            }

            map<string, string> MOVING_EGO::ConfigurationSummary::GetState() {
                map<string, string> statemap;
                statemap["Name"] = "Efficient Global Optimization (EGO)";
                statemap["Kernel"] = opt_->settings_->parameters().ego_kernel;
                statemap["Acquisition function"] = opt_->settings_->parameters().ego_af;
                statemap["AF Optimizer"] = "PSO";
                statemap["Mode"] =
                        opt_->mode_ == Settings::Optimizer::OptimizerMode::Maximize ? "Maximize" : "Minimize";
                statemap["Max Evaluations"] = boost::lexical_cast<string>(opt_->max_evaluations_);
                statemap["Num. initial guesses"] = boost::lexical_cast<string>(opt_->n_initial_guesses_);

                string constraints_used = "";
                for (auto cons : opt_->constraint_handler_->constraints()) {
                    constraints_used += cons->name() + " ";
                }
                statemap["Constraints"] = constraints_used;
                return statemap;
            }

            QUuid MOVING_EGO::ConfigurationSummary::GetId() {
                return QUuid(); // Null UUID
            }

            map<string, vector<double>> MOVING_EGO::ConfigurationSummary::GetValues() {
                map<string, vector<double>> valmap;
                return valmap;
            }

            void MOVING_EGO::updateEvolutionPath(){
                ps_ = (1.0-cs_)*ps_ + sqrt(cs_*(2.0-cs_)*mueff_) * invsqrtC_ * (xmean_- xold_) / sigma_;
                hsig_ = ps_.norm()/sqrt(pow(1.0-(1.0-cs_),2.0*case_handler_->EvaluatedCases().size()/lambda_))/chiN_ < 1.4 + 2.0/(n_vars_+1);
                pc_ = (1.0-cc_)*pc_ + hsig_ * sqrt(cc_*(2.0-cc_)*mueff_) * (xmean_ - xold_) / sigma_;
            }
            ;
            void MOVING_EGO::adaptCovarianceMatrix(){
                Eigen::MatrixXd argument_matrix(n_vars_, int(mu_));
                for(int i = 0; i < n_vars_; i++){
                    double temp = 0;
                    for(int j = 0; j < mu_; j++){
                        argument_matrix(i, j) = population_[j].erands_norm_[i];
                    }
                }

                Eigen::VectorXd temp_weights(weights_.size());
                for(int i = 0; i < weights_.size(); i++){
                    temp_weights(i) = weights_[i];
                }
                auto artmp = (1/sigma_) * (argument_matrix) - xold_.replicate(1,int(mu_));
                C_ = (1-c1_-cmu_) * C_ + c1_ * (pc_ * pc_.transpose() + (1 - hsig_) * cc_ * (2-cc_) * C_) + cmu_ * artmp * temp_weights.asDiagonal() * artmp.transpose();

                sigma_ = sigma_ * exp((cs_/damps_)*(ps_.norm()/chiN_ - 1));
            }

            void MOVING_EGO::decompositionOfC(){
                if(case_handler_->EvaluatedCases().size() - eigeneval_ > lambda_/(c1_+cmu_)/n_vars_/10.0){
                    eigeneval_ = case_handler_->EvaluatedCases().size();
                    const IOFormat fmt(10, DontAlignCols, "\t", " ", "\n", "", "", "");
                    Eigen::MatrixXd Cupper = Eigen::MatrixXd::Zero(n_vars_, n_vars_);
                    for(int i = 0; i < n_vars_; i++){
                        for(int j = i; j < n_vars_; j++){
                            Cupper(i,j) = C_(i,j);
                        }
                    }
                    Eigen::MatrixXd Cupperest = Eigen::MatrixXd::Zero(n_vars_, n_vars_);
                    for(int i = 0; i < n_vars_; i++){
                        for(int j = i+1; j < n_vars_; j++){
                            Cupperest(i,j) = C_(i,j);
                        }
                    }

                    C_ = Cupper + Cupperest.transpose();

                    es_.compute(C_);
                    D_ = es_.eigenvalues().real();
                    B_ = es_.eigenvectors().real();


                    VectorXd D_diag_sqrt;
                    D_diag_sqrt.resize(D_.size());
                    for(int i = 0; i < D_.size(); i++){
                        D_diag_sqrt(i) = sqrt(D_(i));
                    }

                    VectorXd D_diag_inverse;
                    D_diag_inverse.resize(D_.size());
                    for(int i = 0; i < D_.size(); i++){
                        D_diag_inverse(i) = 1./D_(i);
                    }

                    D_ = D_diag_sqrt;
                    invsqrtC_ = B_ * D_diag_inverse.asDiagonal() * B_.transpose();
                    //cout << D_.format(fmt) << endl;

                }
            }

        }
    }
}