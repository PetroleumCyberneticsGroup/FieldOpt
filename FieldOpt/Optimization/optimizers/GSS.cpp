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
#include "GSS.h"
#include "Utilities/math.hpp"

namespace Optimization {
    namespace Optimizers {

        GSS::GSS(Settings::Optimizer *settings, Case *base_case,
                 Model::Properties::VariablePropertyContainer *variables, Reservoir::Grid::Grid *grid)
                : Optimizer(settings, base_case, variables, grid) {
            step_tol_ = settings->parameters().minimum_step_length;
            // \TODO Contr fac -> Set in subclass
            // \TODO Expan fac -> Set in subclass
            // \TODO Directions -> Set in subclass
            // \TODO Step length -> Resize to directions size, set all to initial step length
        }

        void GSS::expand(vector<int> dirs) {
            if (dirs.size() == 0) {
                step_lengths_ = step_lengths_ * expan_fac_;
            }
            else {
                for (int dir : dirs)
                    step_lengths_(dir) = step_lengths_(dir) * expan_fac_;
            }
        }

        void GSS::contract(vector<int> dirs) {
            if (dirs.size() == 0) {
                step_lengths_ = step_lengths_ * contr_fac_;
            }
            else {
                for (int dir : dirs)
                    step_lengths_(dir) = step_lengths_(dir) * contr_fac_;
            }
        }

        QList<Case *> GSS::generate_trial_points(vector<int> dirs) {
            auto trial_points = QList<Case *>();
            if (dirs.size() == 0)
                dirs = range(0, (int)directions_.size(), 1);

            VectorXi int_base = tentative_best_case_->GetIntegerVarVector();
            VectorXd rea_base = tentative_best_case_->GetRealVarVector();

            for (int dir : dirs) {
                auto trial_point = new Case(tentative_best_case_);
                if (int_base.size() > 0) {
                    trial_point->SetIntegerVarValues(perturb(int_base, dir));
                }
                else if (rea_base.size() > 0) {
                    trial_point->SetRealVarValues(perturb(rea_base, dir));
                }
                trial_points.append(trial_point);
            }
            return trial_points;
        }

        template<typename T>
        Matrix<T, Dynamic, 1> GSS::perturb(Matrix<T, Dynamic, 1> base, int dir) {
            Matrix<T, Dynamic, 1> perturbation = base + directions_[dir].cast<T>() * step_lengths_[dir];
            return perturbation;
        }

        bool GSS::is_converged() {
            for (int i = 0; i < step_lengths_.size(); ++i) {
                if (step_lengths_(i) > step_tol_)
                    return false;
            }
            return true;
        }

    }
}
