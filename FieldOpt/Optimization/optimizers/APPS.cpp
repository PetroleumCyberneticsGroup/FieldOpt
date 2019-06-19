/******************************************************************************
   Created by einar on 11/21/16.
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>

   Modified by Einar J. M. Baumann <einar.baumann@gmail.com> 02/19/19.

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
#include "Utilities/printer.hpp"
#include "Utilities/verbosity.h"

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
    if (penalize_) {
        if (!normalizer_ofv_.is_ready()) initializeNormalizers();

        // penalize the base case
        double pen_ofv = PenalizedOFV(tentative_best_case_);
        tentative_best_case_->set_objective_function_value(pen_ofv);
    }
    if (enable_logging_) {
        logger_->AddEntry(this);
    }
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
}

void APPS::handleEvaluatedCase(Case *c) {
    if (isImprovement(c)) successful_iteration(c);
    else unsuccessful_iteration(c);
}

void APPS::successful_iteration(Case *c) {
    updateTentativeBestCase(c);
    set_step_lengths(c->origin_direction_index(), c->origin_step_length());
    expand();
    reset_active();
    prune_queue();
    if (VERB_OPT >= 2) print_state("Successful iteration");
    iterate();
}

void APPS::unsuccessful_iteration(Case *c) {
    vector<int> unsuccessful_direction;
    if (c->origin_case()->id() == GetTentativeBestCase()->id()) {
        unsuccessful_direction.push_back(c->origin_direction_index());
        set_inactive(unsuccessful_direction);
        contract(unsuccessful_direction);
    }
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
    std::stringstream ss;
    ss << header << "|";
    ss << "Step lengths  : " << vec_to_str(vector<double>(step_lengths_.data(), step_lengths_.data() + step_lengths_.size())) << "|";
    ss << "Iteration: " << iteration_ << "|";
    ss << "Current best case: " << tentative_best_case_->id().toString().toStdString() << "|";
    ss << "              OFV: " << tentative_best_case_->objective_function_value();
    Printer::ext_info(ss.str(), "Optimization", "APPS");
}
}
}
