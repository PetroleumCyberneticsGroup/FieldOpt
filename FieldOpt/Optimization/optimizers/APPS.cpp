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
#include "APPS.h"
#include "gss_patterns.hpp"

namespace Optimization {
    namespace Optimizers {

        APPS::APPS(Settings::Optimizer *settings, Case *base_case,
                   Model::Properties::VariablePropertyContainer *variables, Reservoir::Grid::Grid *grid)
                : GSS(settings, base_case, variables, grid) {
            directions_ = GSSPatterns::Compass(num_vars_);
            step_lengths_ = Eigen::VectorXd(directions_.size());
            step_lengths_.fill(settings->parameters().initial_step_length);
            max_queue_length_ = directions_.size() * 2;
        }

        void APPS::iterate() {
            case_handler_->AddNewCases(generate_trial_points(inactive()));
            set_active(inactive());
        }

        void APPS::handleEvaluatedCase(Case *c) {
            if (isImprovement(c)) successful_iteration(c);
            else unsuccessful_iteration(c);
        }

        void APPS::successful_iteration(Case *c) {
            tentative_best_case_ = c;
            set_step_lengths(c->origin_step_length());
            reset_active();
            prune_queue();
            iterate();
        }

        void APPS::unsuccessful_iteration(Case *c) {
            vector<int> unsuccessful_direction = vector<int>(0);
            if (c->origin_case() == tentative_best_case_) {
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
                active_.erase(dir);
        }

        void APPS::reset_active() {
            active_.clear();
        }

        vector<int> APPS::inactive() {
            vector<int> inactive;
            for (int i = 0; i < directions_.size(); ++i) {
                if (active_.count(i) == 0) inactive.push_back(i);
            }
            return inactive;
        }

        void APPS::prune_queue() {
            if (case_handler_->QueuedCases().size() < max_queue_length_ + directions_.size())
                return;
            else {
                while (case_handler_->QueuedCases().size() > max_queue_length_ + directions_.size()) {
                    case_handler_->QueuedCases().pop_back();
                }
                return;
            }
        }
    }
}
