/******************************************************************************
   Created by einar on 11/21/16.
   Copyright (C) 2016 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include <iostream>
#include "APPS.h"
#include "gss_patterns.hpp"
#include "Utilities/stringhelpers.hpp"

namespace Optimization {
namespace Optimizers {

APPS::APPS(Settings::Optimizer *settings,
           Case *base_case,
           Model::Properties::VariablePropertyContainer *variables,
           Reservoir::Grid::Grid *grid,
           Logger *logger,
           CaseHandler *case_handler,
           Constraints::ConstraintHandler *constraint_handler
)
    : GSS(settings, base_case, variables, grid, logger, case_handler, constraint_handler) {


    assert(settings->parameters().max_queue_size >= 1.0);
    max_queue_length_ = directions_.size() * settings->parameters().max_queue_size;
    is_async_ = true;
//            iterate();
}

void APPS::iterate() {
    if (enable_logging_) {
        logger_->AddEntry(this);
    }
    if (inactive().size() > 0) {
        case_handler_->AddNewCases(generate_trial_points(inactive()));
        set_active(inactive());
    }
    iteration_++;
    if (verbosity_level_ >= 1) print_state("ITERATION START");
}

void APPS::handleEvaluatedCase(Case *c) {
    evaluated_cases_++;
    if (isImprovement(c)) successful_iteration(c);
    else unsuccessful_iteration(c);
}

void APPS::successful_iteration(Case *c) {
    updateTentativeBestCase(c);
    set_step_lengths(c->origin_step_length());
    expand();
    reset_active();
    prune_queue();
    if (verbosity_level_ >= 1) print_state("SUCCESSFUL ITERATION");
    iterate();
}

void APPS::unsuccessful_iteration(Case *c) {
    vector<int> unsuccessful_direction;
    if (c->origin_case()->id() == GetTentativeBestCase()->id()) {
        unsuccessful_direction.push_back(c->origin_direction_index());
        set_inactive(unsuccessful_direction);
        contract(unsuccessful_direction);
    }
    if (verbosity_level_ >= 1) print_state("UNSUCCESSFUL ITERATION");
    if (!is_converged()) iterate();
}

void APPS::set_active(vector<int> dirs) {
    for (int dir : dirs)
        active_.insert(dir);
}

void APPS::set_inactive(vector<int> dirs) {
    for (int dir : dirs)
        if (active_.count(dir) > 0)
            active_.erase(dir);
}

void APPS::reset_active() {
    active_.clear();
}

vector<int> APPS::inactive() {
    vector<int> inactive;
    for (int i = 0; i < directions_.size(); ++i) {
        if (active_.count(i) == 0 && step_lengths_(i) >= step_tol_(i))
            inactive.push_back(i);
    }
    return inactive;
}

void APPS::prune_queue() {
    if (case_handler_->QueuedCases().size() <= max_queue_length_ - directions_.size())
        return;
    else {
        while (case_handler_->QueuedCases().size() > max_queue_length_ - directions_.size()) {
            auto dequeued_case = dequeue_case_with_worst_origin();
            if (dequeued_case->origin_case()->id() == GetTentativeBestCase()->id())
                set_inactive(vector<int>{dequeued_case->origin_direction_index()});
        }
        return;
    }
}

void APPS::print_state(string header) {
    cout << "APPS state (" << header << ")" << "---------"<< endl;
    cout << "step_lengths_  : " << vec_to_str(vector<double>(step_lengths_.data(), step_lengths_.data() + step_lengths_.size())) << endl;
    cout << "active_        : " << vec_to_str(vector<int>(active_.begin(), active_.end())) << endl;
    cout << "inactive()     : " << vec_to_str(inactive()) << endl;
    cout << "queue size     : " << case_handler_->QueuedCases().size() << endl;

    cout << "best case origin:" << endl;
    cout << " direction idx : " << GetTentativeBestCase()->origin_direction_index() << endl;
    cout << " step length   : " << GetTentativeBestCase()->origin_step_length() << endl;
}
}
}
