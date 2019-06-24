/******************************************************************************
    Created by Brage on 18/06/19.
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

#include "CMA_ES.h"
#include "Utilities/math.hpp"
#include "Utilities/random.hpp"
#include "Utilities/stringhelpers.hpp"
#include "Settings/optimizer.h"
#include <math.h>
#include <random>

namespace Optimization {
namespace Optimizers {
CMA_ES::CMA_ES(Settings::Optimizer *settings,
               Case *base_case,
               Model::Properties::VariablePropertyContainer *variables,
               Reservoir::Grid::Grid *grid,
               Logger *logger,
               CaseHandler *case_handler,
               Constraints::ConstraintHandler *constraint_handler
) : Optimizer(settings, base_case, variables, grid, logger, case_handler, constraint_handler) {
    settings_ = settings;
    // User defined parameters (need to be edited)
    n_vars_ = variables->ContinousVariableSize();
    std::random_device rd{};
    std::normal_distribution<> d(0.0, 1.0);
    std::mt19937 gen{rd()};

    improve_base_case_ = settings->parameters().improve_base_case;

    penalty_ = 1.1;
    gen_ = get_random_generator(settings->parameters().rng_seed);
    max_iterations_ = settings->parameters().max_generations;

    population_size_ = settings->parameters().population_size;
    sigma_ = 0.3;
    xmean_ = Eigen::VectorXd::Zero(n_vars_);

    if (constraint_handler_->HasBoundaryConstraints()) {
        lower_bound_ = constraint_handler_->GetLowerBounds(base_case->GetRealVarIdVector());
        upper_bound_ = constraint_handler_->GetUpperBounds(base_case->GetRealVarIdVector());
    } else {
        lower_bound_.resize(n_vars_);
        upper_bound_.resize(n_vars_);
        lower_bound_.fill(settings->parameters().lower_bound);
        upper_bound_.fill(settings->parameters().upper_bound);
    }

    Eigen::VectorXd base_case_variables = base_case->GetRealVarVector();
    if (improve_base_case_ == true) {
        for (int i = 0; i < n_vars_; i++) {
            xmean_(i) = (base_case_variables(i) - lower_bound_(i)) / (upper_bound_(i) - lower_bound_(i));
        }
    } else {
        for (int i = 0; i < n_vars_; i++) {
            xmean_(i) = random_doubles(gen_, 0, 1, 1)[0];
        }
    }

    //Strategy parameter setting: Selection
    if (population_size_ != -1){
        lambda_ = population_size_;
    } else {
        lambda_ = 4 + floor(3 * log(n_vars_));
    }
    mu_ = lambda_ / 2;
    for (int i = 1; i <= mu_; i++) {
        weights_.push_back(log(mu_ + 0.5) - log(i));
    }
    mu_ = floor(mu_);
    double sum_of_weights = 0;
    for (int i = 0; i < weights_.size(); i++) {
        sum_of_weights += weights_[i];
    }

    for (int i = 0; i < weights_.size(); i++) {
        weights_[i] = weights_[i] / sum_of_weights;
    }
    double temp_sum_of_weights = 0;
    double temp_sum_of_variance_of_weights = 0;
    for (int i = 0; i < weights_.size(); i++) {
        temp_sum_of_weights += weights_[i];
        temp_sum_of_variance_of_weights += pow(weights_[i], 2);
    }

    mueff_ = temp_sum_of_weights / temp_sum_of_variance_of_weights;
    //Strategy parameter setting: Adaptation
    cc_ = (4 + mueff_ / n_vars_) / (n_vars_ + 4 + 2 * mueff_ / n_vars_);
    cs_ = (mueff_ + 2) / (n_vars_ + mueff_ + 5);
    c1_ = 2 / (pow(n_vars_ + 1.3, 2) + mueff_);

    if (1 - c1_ < 2 * (mueff_ - 2 + 1 / mueff_) / (pow(n_vars_ + 2, 2) + mueff_)) {
        cmu_ = 1 - c1_;
    } else {
        cmu_ = 2 * (mueff_ - 2 + 1 / mueff_) / (pow(n_vars_ + 2, 2) + mueff_);
    }

    if (0 > sqrt((mueff_ - 1) / (n_vars_ + 1)) - 1) {
        damps_ = 1 + 2 * 0 + cs_;
    } else {
        damps_ = 1 + 2 * sqrt((mueff_ - 1) / (n_vars_ + 1)) - 1 + cs_;
    }

    // Initialize dynamic (internal) strategy parameters and constants
    pc_ = Eigen::VectorXd::Zero(n_vars_);
    ps_ = Eigen::VectorXd::Zero(n_vars_);
    B_ = Eigen::MatrixXd::Identity(n_vars_, n_vars_);
    D_ = Eigen::VectorXd::Ones(n_vars_);
    Eigen::VectorXd temp_D = Eigen::VectorXd::Zero(n_vars_);
    for (int i = 0; i < D_.size(); i++) {
        temp_D[i] = D_(i) * D_(i);
    }
    C_ = B_ * (temp_D).asDiagonal() * B_.transpose();
    for (int i = 0; i < D_.size(); i++) {
        temp_D[i] = 1.0 / D_(i);
    }
    invsqrtC_ = B_ * (temp_D).asDiagonal() * B_.transpose();
    eigeneval_ = 0;
    chiN_ = pow(n_vars_, 0.5) * (1 - (float(1) / (4 * n_vars_)) + 1 / (21 * pow(n_vars_, 2)));
    for (int i = 0; i < lambda_; ++i) {
        auto new_case = generateCase(xmean_, i, 1);
        case_handler_->AddNewCase(new_case);
    }
}

CMA_ES::Individual::Individual(Optimization::Case *c, boost::random::mt19937 &gen, int index,
                               Eigen::VectorXd erands_norm, double penalty_dist) {
    case_pointer_ = c;
    rea_vars_ = c->GetRealVarVector();
    erands_norm_ = erands_norm;
    index_ = index;
    penalty_dist_ = penalty_dist;
}

vector<CMA_ES::Individual> CMA_ES::sortPopulation(vector<CMA_ES::Individual> population) {
    vector<CMA_ES::Individual> sortedPopulation;
    int n = population_.size();
    bool swapped = true;
    while (swapped) {
        swapped = false;
        for (int i = 1; i <= n - 1; i++) {
            if (population[i - 1].ofv() > population[i].ofv() &&
                (Settings::Optimizer::Minimize == settings_->mode())) {
                CMA_ES::Individual temp_individual = population[i];
                population[i] = population[i - 1];
                population[i - 1] = temp_individual;
                swapped = true;
            } else if (population[i - 1].ofv() < population[i].ofv() &&
                       (Settings::Optimizer::Maximize == settings_->mode())) {
                CMA_ES::Individual temp_individual = population[i];
                population[i] = population[i - 1];
                population[i - 1] = temp_individual;
                swapped = true;
            }
        }
    }
    return population;
}

void CMA_ES::iterate() {
    if (enable_logging_) {
        logger_->AddEntry(this);
    }
    for (int i = 0; i < population_.size(); i++) {
        if (Settings::Optimizer::Minimize == settings_->mode()) {
            population_[i].case_pointer_->set_objective_function_value(
                    population_[i].ofv() + (exp(penalty_ * population_[i].penalty_dist_) - 1));
        } else {
            population_[i].case_pointer_->set_objective_function_value(
                    population_[i].ofv() - (exp(penalty_ * population_[i].penalty_dist_) - 1));
        }
    }
    population_ = sortPopulation(population_);
    xold_ = xmean_;
    xmean_ = Eigen::VectorXd::Zero(xmean_.size());
    for (int i = 0; i < n_vars_; i++) {
        double temp = 0;
        for (int j = 0; j < mu_; j++) {
            temp += population_[j].erands_norm_[i] * weights_[j];
        }
        xmean_(i) = temp;
    }
    updateEvolutionPath();
    adaptCovarianceMatrix();
    decompositionOfC();

    for (int i = 0; i < lambda_; ++i) {
        auto new_case = generateCase(xmean_, i, 0);
        case_handler_->AddNewCase(new_case);
    }
    population_ = temp_population_;
    iteration_++;
}

void CMA_ES::handleEvaluatedCase(Case *c) {
    if (isImprovement(c)) {
        updateTentativeBestCase(c);
        if (VERB_OPT > 0) {
            stringstream ss;
            ss.precision(6);
            ss << scientific;
            ss << "New best in population, iteration " << Printer::num2str(iteration_) << ": OFV "
               << c->objective_function_value();
            Printer::ext_info(ss.str(), "Optimization", "CMA_ES");
        }
    }
}

Optimizer::TerminationCondition CMA_ES::IsFinished() {
    if (case_handler_->CasesBeingEvaluated().size() > 0) return NOT_FINISHED;
    if (iteration_ < max_iterations_) return NOT_FINISHED;
    else return MAX_EVALS_REACHED;
}

void CMA_ES::updateEvolutionPath() {
    ps_ = (1.0 - cs_) * ps_ + sqrt(cs_ * (2.0 - cs_) * mueff_) * invsqrtC_ * (xmean_ - xold_) / sigma_;
    hsig_ = ps_.norm() / sqrt(pow(1.0 - (1.0 - cs_), 2.0 * case_handler_->EvaluatedCases().size() / lambda_)) /
            chiN_ < 1.4 + 2.0 / (n_vars_ + 1);
    pc_ = (1.0 - cc_) * pc_ + hsig_ * sqrt(cc_ * (2.0 - cc_) * mueff_) * (xmean_ - xold_) / sigma_;
}

void CMA_ES::adaptCovarianceMatrix() {
    Eigen::MatrixXd argument_matrix(n_vars_, int(mu_));
    for (int i = 0; i < n_vars_; i++) {
        double temp = 0;
        for (int j = 0; j < mu_; j++) {
            argument_matrix(i, j) = population_[j].erands_norm_[i];
        }
    }
    Eigen::VectorXd temp_weights(weights_.size());
    for (int i = 0; i < weights_.size(); i++) {
        temp_weights(i) = weights_[i];
    }
    auto artmp = (1 / sigma_) * (argument_matrix) - xold_.replicate(1, int(mu_));
    C_ = (1 - c1_ - cmu_) * C_ + c1_ * (pc_ * pc_.transpose() + (1 - hsig_) * cc_ * (2 - cc_) * C_) +
         cmu_ * artmp * temp_weights.asDiagonal() * artmp.transpose();
    sigma_ = sigma_ * exp((cs_ / damps_) * (ps_.norm() / chiN_ - 1));
}

void CMA_ES::decompositionOfC() {
    if (case_handler_->EvaluatedCases().size() - eigeneval_ > lambda_ / (c1_ + cmu_) / n_vars_ / 10.0) {
        eigeneval_ = case_handler_->EvaluatedCases().size();
        const IOFormat fmt(10, DontAlignCols, "\t", " ", "\n", "", "", "");
        Eigen::MatrixXd Cupper = Eigen::MatrixXd::Zero(n_vars_, n_vars_);
        for (int i = 0; i < n_vars_; i++) {
            for (int j = i; j < n_vars_; j++) {
                Cupper(i, j) = C_(i, j);
            }
        }
        Eigen::MatrixXd Cupperest = Eigen::MatrixXd::Zero(n_vars_, n_vars_);
        for (int i = 0; i < n_vars_; i++) {
            for (int j = i + 1; j < n_vars_; j++) {
                Cupperest(i, j) = C_(i, j);
            }
        }
        C_ = Cupper + Cupperest.transpose();

        es_.compute(C_);
        D_ = es_.eigenvalues().real();
        B_ = es_.eigenvectors().real();

        VectorXd D_diag_sqrt;
        D_diag_sqrt.resize(D_.size());
        for (int i = 0; i < D_.size(); i++) {
            D_diag_sqrt(i) = sqrt(D_(i));
        }

        VectorXd D_diag_inverse;
        D_diag_inverse.resize(D_.size());
        for (int i = 0; i < D_.size(); i++) {
            D_diag_inverse(i) = 1. / D_(i);
        }

        D_ = D_diag_sqrt;
        invsqrtC_ = B_ * D_diag_inverse.asDiagonal() * B_.transpose();
    }
}

Case *CMA_ES::generateCase(Eigen::VectorXd xmean, int index, bool first_iteration) {
    Case *new_case;
    new_case = new Case(GetTentativeBestCase());

    Eigen::VectorXd xmeanNormal(n_vars_);
    Eigen::VectorXd erands_norm(n_vars_);
    Eigen::VectorXd erands(n_vars_);
    double penalty_dist = 0;
    for (int i = 0; i < n_vars_; ++i) {
        xmeanNormal(i) = d(gen);
    }
    const IOFormat fmt(1, DontAlignCols, "\t", " ", "", "", "", "");
    for (int i = 0; i < n_vars_; ++i) {
        erands_norm(i) = xmean(i) + sigma_ * B_(i, i) * (D_(i) * xmeanNormal(i));
        if (erands_norm(i) > 1.0) {
            penalty_dist += abs(erands_norm(i) - 1.0);
        } else if (erands_norm(i) < 0.0) {
            penalty_dist += abs(erands_norm(i));
        } else {
            penalty_dist += 0;
        }
        erands(i) = lower_bound_(i) + erands_norm(i) * (upper_bound_(i) - lower_bound_(i));
        if (erands(i) < lower_bound_(i)) {
            erands(i) = lower_bound_(i);
        } else if (erands(i) > upper_bound_(i)) {
            erands(i) = upper_bound_(i);
        }
    }
    new_case->SetRealVarValues(erands);
    if (first_iteration) {
        population_.push_back(Individual(new_case, gen_, index, erands_norm, penalty_dist));
    } else {
        temp_population_.push_back(Individual(new_case, gen_, index, erands_norm, penalty_dist));
    }
    return new_case;
}
}
}

