/******************************************************************************
 * Created: 16.12.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#include <iostream>
#include "ensemble_helper.h"
#include "Utilities/math.hpp"

namespace Runner {

EnsembleHelper::EnsembleHelper() {
    current_case_ = 0;
    rzn_queue_ = std::vector<std::string>();
    rzn_busy_ = std::vector<std::string>();
}

EnsembleHelper::EnsembleHelper(const Settings::Ensemble &ensemble, int rng_seed) {
    ensemble_ = ensemble;
    current_case_ = 0;
    rzn_queue_ = std::vector<std::string>();
    rzn_busy_ = std::vector<std::string>();
    n_select_ = 10;
    rng_ = get_random_generator(rng_seed*3);

    assert(n_select_ < ensemble.GetAliases().size());
}
void EnsembleHelper::SetActiveCase(Optimization::Case *c) {
    if (!IsCaseDone()) {
        std::cerr << "ERROR: Unable to set new active case before the previous case is done." << std::endl;
        throw std::runtime_error("Error in EnsembleHelper.");
    }

    current_case_ = c;
    selectRealizations();
    eval_start_time_ = std::chrono::high_resolution_clock::now();
}
bool EnsembleHelper::IsCaseDone() const {
    return rzn_queue_.empty() && rzn_busy_.empty();
}
bool EnsembleHelper::IsCaseAvailableForEval() const {
    return !rzn_queue_.empty();
}
Optimization::Case *EnsembleHelper::GetCaseForEval() {
    if (!IsCaseAvailableForEval()) {
        std::cerr << "ERROR: No more cases are available for evaluation." << std::endl;
        throw std::runtime_error("Error in EnsembleHelper.");
    }
    auto case_copy = new Optimization::Case(current_case_);
    std::string next_alias = rzn_queue_.back();
    rzn_queue_.pop_back();
    rzn_busy_.push_back(next_alias);
    case_copy->SetEnsembleRealization(QString::fromStdString(next_alias));// TODO: This is a duplicate case that wont get deleted, i.e. a MEMORY LEAK.
    return case_copy;
}
void EnsembleHelper::SubmitEvaluatedRealization(Optimization::Case *c) {
    long alias_pos = distance(rzn_busy_.begin(), find(rzn_busy_.begin(), rzn_busy_.end(),
                                                     c->GetEnsembleRealization().toStdString()));
    if (alias_pos < 0 || alias_pos >= rzn_busy_.size()) {
        std::cerr << "ERROR: Unable to find alias in list of busy realizations." << std::endl;
        throw std::runtime_error("Error in EnsembleHelper.");
    }

    if (c->state.eval == Optimization::Case::CaseState::E_DONE) {
        current_case_->SetRealizationOfv(c->GetEnsembleRealization(), c->objective_function_value());
    }
    else {
        std::cout << "WARNING: Ensemble realization case "
                  << c->GetEnsembleRealization().toStdString()
                  << " was not successfully evaluated. It will not be further considered."
                  << std::endl;
    }
    rzn_busy_.erase(rzn_busy_.begin() + alias_pos);
}
Optimization::Case *EnsembleHelper::GetEvaluatedCase() {
    if (!IsCaseDone()) {
        std::cerr << "ERROR: Unable to get case before all selected realizations have been evaluated." << std::endl;
        throw std::runtime_error("Error in EnsembleHelper.");
    }
    rzn_queue_ = std::vector<std::string>();
    rzn_busy_ = std::vector<std::string>();
    current_case_->set_objective_function_value(current_case_->GetEnsembleAverageOfv());
    auto eval_end_time = std::chrono::high_resolution_clock::now();
    auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(eval_end_time - eval_start_time_);
    current_case_->SetSimTime(time_diff.count() / 1000);
    current_case_->state.eval = Optimization::Case::CaseState::EvalStatus::E_DONE;
    return current_case_;
}
void EnsembleHelper::selectRealizations() {
    auto all_aliases = ensemble_.GetAliases();
    for (auto alias : all_aliases) {
        rzn_queue_.push_back(alias);
    }
//    auto indices = unique_random_integers(rng_, 0, all_aliases.size() - 1, n_select_);
//
//    for (auto idx : indices) {
//        rzn_queue_.push_back(all_aliases[idx]);
//    }
}
Settings::Ensemble::Realization EnsembleHelper::GetRealization(const std::string &alias) const {
    return ensemble_.GetRealization(alias);
}
Settings::Ensemble::Realization EnsembleHelper::GetBaseRealization() const {
    auto base_alias = ensemble_.GetAliases()[0];
    return ensemble_.GetRealization(base_alias);
}
int EnsembleHelper::NBusyCases() const {
    return rzn_busy_.size();
}
int EnsembleHelper::NQueuedCases() const {
    return rzn_queue_.size();
}
std::string EnsembleHelper::GetStateString() const {
    std::stringstream str;
    str << "EnsembleHelper: ";
    if (current_case_ == 0) {
        str << "Case not set.";
    }
    else {
        str << "Current case done: " << (IsCaseDone() ? "Yes" : "No") << std::endl;
        str << "                N. Queued Cases: " << NQueuedCases();
        str << "                N. Busy Cases:   " << NBusyCases();
    }
    return str.str();
}
}
