/******************************************************************************
   Created by einar on 1/10/19.
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "VFSA.h"
#include <cmath>
#include "Utilities/math.hpp"

namespace Optimization {
namespace Optimizers {

VFSA::VFSA(Settings::Optimizer *settings,
           Optimization::Case *base_case,
           Model::Properties::VariablePropertyContainer *variables,
           Reservoir::Grid::Grid *grid,
           Logger *logger,
           Optimization::CaseHandler *case_handler,
           Optimization::Constraints::ConstraintHandler *constraint_handler)
    : Optimizer(settings, base_case, variables, grid, logger, case_handler, constraint_handler)
{
    /*
     * TODO: Settings parameters
     * - evals_pr_iteration
     * - initial_temperature
     * - max_iterations
     * - parallel
     */

    D_ = base_case->GetRealVarIdVector().size();
    T_.resize(D_);
    c_.resize(D_);

    parallel_ = settings->parameters().vfsa_parallel;
    max_iterations_ = settings->parameters().vfsa_max_iterations;
    evals_pr_iteration_ = settings->parameters().vfsa_evals_pr_iteration;
    T_.fill(settings->parameters().vfsa_init_temp);
    c_.fill(settings->parameters().vfsa_temp_scale);
    gen_ = get_random_generator(settings->parameters().rng_seed);
    if (constraint_handler_->HasBoundaryConstraints()) {
        min_ = constraint_handler_->GetLowerBounds(base_case->GetRealVarIdVector());
        max_ = constraint_handler_->GetUpperBounds(base_case->GetRealVarIdVector());
    }
    else {
        min_.resize(D_);
        max_.resize(D_);
        min_.fill(settings->parameters().lower_bound);
        max_.fill(settings->parameters().upper_bound);
    }

    evals_in_iteration_ = 0;
}
Optimization::Optimizer::TerminationCondition VFSA::IsFinished() {
    if (case_handler_->CasesBeingEvaluated().size() > 0 || case_handler_->QueuedCases().size() > 0) {
        return NOT_FINISHED;
    }
    else if (iteration_ <= max_iterations_) {
        return NOT_FINISHED;
    }
    else {
        return MAX_ITERATIONS_REACHED;
    }
}
void VFSA::handleEvaluatedCase(Optimization::Case *c) {
    if (isImprovement(c)) { // Update tentative best case if improvement
        if (VERB_OPT >= 1) {
            Printer::ext_info("Found new best case in iteration " + Printer::num2str(iteration_) + ". OFV: "
                                  + Printer::num2str(c->objective_function_value()), "Optimization", "VFSA");
        }
        updateTentativeBestCase(c);
    }
    else { // If not improvement, update anyway if selection probability is greater than a random number [0, 1].
        double sel_prop = selectionProbability(tentative_best_case_->objective_function_value(),
                                               c->objective_function_value());
        if (sel_prop > random_double(gen_)) {
            updateTentativeBestCase(c);
            if (VERB_OPT >= 1) {
                Printer::ext_info("Updated tent. best with other case randomly. New OFV: "
                                      + Printer::num2str(c->objective_function_value()), "Optimization", "VFSA");
            }
        }
    }
    evals_in_iteration_++;
}
void VFSA::iterate() {
    if (evals_in_iteration_ == evals_pr_iteration_) {
        iteration_++;
        if (VERB_OPT >= 3) Printer::ext_info("Starting iteration " + Printer::num2str(iteration_), "Optimization", "VFSA");
        evals_in_iteration_ = 0;
    }
    if (parallel_ = false) { // Serial mode
        case_handler_->AddNewCase(createPerturbation());
        return;
    }
    else { // Parallel mode
        for (int i = 0; i < evals_pr_iteration_; ++i) {
            case_handler_->AddNewCase(createPerturbation());
        }
        return;
    }
}

double VFSA::selectionProbability(const double old_ofv, const double new_ofv) const {
    double difference;
    if (mode_ == Settings::Optimizer::OptimizerMode::Maximize) {
        difference = old_ofv - new_ofv;
    }
    else {
        difference = new_ofv - old_ofv;
    }
    return exp(-1.0 * difference / T_.norm());
}

Eigen::VectorXd VFSA::nextTemperature(const Eigen::VectorXd old_T) const {
    Eigen::VectorXd new_T = Eigen::VectorXd(old_T.size());
    for (int i = 0; i < old_T.size(); ++i) {
        new_T[i] = old_T[i] * exp(-c_[i] * pow(iteration_, 1.0/D_));
    }
    return new_T;
}

Eigen::VectorXd VFSA::updatingFactors(const Eigen::VectorXd T) {
    Eigen::VectorXd ys = Eigen::VectorXd(T.size());
    auto rands = random_doubles(gen_, -1.0, 1.0, D_);
    double u;
    for (int i = 0; i < T.size(); ++i) {
        u = rands[i];
        double sign = u - 0.5 > 0.0 ? 1.0 : -1.0;
        ys[i] = sign * T[i] * ( pow(1 + 1/T[i], abs(2*u-1)) - 1 );
    }
    return ys;
}

Case * VFSA::createPerturbation() {
    Eigen::VectorXd old = tentative_best_case_->GetRealVarVector();
    Eigen::VectorXd ys = updatingFactors(T_);

    Eigen::VectorXd step = ys.cwiseProduct(max_ - min_);
    Eigen::VectorXd new_vars = old + step;
    Case *new_case = new Case(tentative_best_case_);
    new_case->SetRealVarValues(new_vars);
    constraint_handler_->SnapCaseToConstraints(new_case);
    return new_case;
}

}
}

