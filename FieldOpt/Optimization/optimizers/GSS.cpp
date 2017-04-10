/******************************************************************************
   Created by einar on 11/3/16.
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
#include "GSS.h"
#include "Utilities/math.hpp"

namespace Optimization {
    namespace Optimizers {

        GSS::GSS(Settings::Optimizer *settings,
                 Case *base_case,
                 Model::Properties::VariablePropertyContainer *variables,
                 Reservoir::Grid::Grid *grid,
                 Logger *logger
        )
                : Optimizer(settings, base_case, variables, grid, logger) {

            int numRvars = base_case->GetRealVarVector().size();
            int numIvars = base_case->GetIntegerVarVector().size();
            num_vars_ = numRvars + numIvars;
            if (numRvars > 0 && numIvars > 0)
                std::cout << ("WARNING: Compass search does not handle both continuous and discrete variables at the same time");

            contr_fac_ = settings->parameters().contraction_factor;
            assert(contr_fac_ < 1.0);

            expan_fac_ = settings->parameters().expansion_factor;
            assert(expan_fac_ >= 1.0);

            step_tol_ = settings->parameters().minimum_step_length;
            assert(step_lengths_.size() == directions_.size());
        }

        Optimizer::TerminationCondition GSS::IsFinished()
        {
            TerminationCondition tc = NOT_FINISHED;
            if (case_handler_->EvaluatedCases().size() >= max_evaluations_)
                tc = MAX_EVALS_REACHED;
            else if (is_converged())
                tc = MINIMUM_STEP_LENGTH_REACHED;

            if (tc != NOT_FINISHED) {
                logger_->AddEntry(new Summary(this, tc));
            }
            return tc;
        }

        void GSS::expand(vector<int> dirs) {
            if (dirs[0] == -1) {
                step_lengths_ = step_lengths_ * expan_fac_;
            }
            else {
                for (int dir : dirs)
                    step_lengths_(dir) = step_lengths_(dir) * expan_fac_;
            }
        }

        void GSS::contract(vector<int> dirs) {
            if (dirs[0] == -1) {
                step_lengths_ = step_lengths_ * contr_fac_;
            }
            else {
                for (int dir : dirs)
                    step_lengths_(dir) = step_lengths_(dir) * contr_fac_;
            }
        }

        QList<Case *> GSS::generate_trial_points(vector<int> dirs) {
            auto trial_points = QList<Case *>();
            if (dirs[0] == -1)
                dirs = range(0, (int)directions_.size(), 1);

            VectorXi int_base = GetTentativeBestCase()->GetIntegerVarVector();
            VectorXd rea_base = GetTentativeBestCase()->GetRealVarVector();

            for (int dir : dirs) {
                auto trial_point = new Case(GetTentativeBestCase());
                if (int_base.size() > 0) {
                    trial_point->SetIntegerVarValues(perturb(int_base, dir));
                }
                else if (rea_base.size() > 0) {
                    trial_point->SetRealVarValues(perturb(rea_base, dir));
                }
                trial_point->set_origin_data(GetTentativeBestCase(), dir, step_lengths_(dir));
                trial_points.append(trial_point);
            }

            for (Case *c : trial_points)
                constraint_handler_->SnapCaseToConstraints(c);

            return trial_points;
        }

        template<typename T>
        Matrix<T, Dynamic, 1> GSS::perturb(Matrix<T, Dynamic, 1> base, int dir) {
            Matrix<T, Dynamic, 1> perturbation = base + directions_[dir].cast<T>() * step_lengths_(dir);
            return perturbation;
        }

        bool GSS::is_converged() {
            for (int i = 0; i < step_lengths_.size(); ++i) {
                if (step_lengths_(i) >= step_tol_)
                    return false;
            }
            return true;
        }

        void GSS::set_step_lengths(double len) {
            step_lengths_.fill(len);
        }

        Case * GSS::dequeue_case_with_worst_origin() {
            auto queued_cases = case_handler_->QueuedCases();
            std::sort(queued_cases.begin(), queued_cases.end(),
                      [this](const Case *c1, const Case *c2) -> bool {
                          return isBetter(c1, c2);
                      });
            case_handler_->DequeueCase(queued_cases.last()->id());
            return queued_cases.last();
        }

    }
}
