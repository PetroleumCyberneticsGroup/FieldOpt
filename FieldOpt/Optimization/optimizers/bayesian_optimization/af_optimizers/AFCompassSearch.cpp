/******************************************************************************
   Created by einar on 6/8/17.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include <Utilities/math.hpp>
#include "optimizers/gss_patterns.hpp"
#include "AFCompassSearch.h"
namespace Optimization {
namespace Optimizers {
namespace BayesianOptimization {
namespace AFOptimizers {
Optimization::Optimizers::BayesianOptimization::AFOptimizers::AFCompassSearch::AFCompassSearch() {}
AFCompassSearch::AFCompassSearch(const VectorXd &lb, const VectorXd &ub) : lb_(lb), ub_(ub) {
    gen_ = get_random_generator();
    step_lengths_ = 0.25 * (ub - lb);
    min_step_lengths_ = step_lengths_ / 100.0;
    directions_ = ::Optimization::GSSPatterns::Compass(lb_.size());

    cout << step_lengths_ << endl;
    cout << min_step_lengths_ << endl;
}
Eigen::VectorXd AFCompassSearch::Optimize(libgp::GaussianProcess *gp, AcquisitionFunction &af, double target) {
    VectorXd best_point = generateRandomVector();
    double best_afv = af.Evaluate(gp, best_point, 0);
    int n_restarts = lb_.size() * 4; // Number of runs to make

    for (int r = 0; r < n_restarts; ++r) {
        step_lengths_ = 0.25 * (ub_ - lb_); // reset step lengths
        VectorXd  best_point_run = generateRandomVector(); // Best point in this run
        double target_run = gp->f(best_point.data()); // Target in current run
        double best_afv_run = af.Evaluate(gp, best_point, 0);

        while (true) {
            vector<VectorXd> perturbations;
            for (int i = 0; i < lb_.size(); ++i) { // Generate new perturbations
                VectorXd pert = best_point + directions_[i].cast<double>() * step_lengths_(i);
                for (int k = 0; k < lb_.size(); ++k) { // Snap to bounds if necessary
                    if (pert(k) < lb_(k))
                        pert(k) = lb_(k);
                    else if (pert(k) > ub_(k))
                        pert(k) = ub_(k);
                }
                perturbations.push_back(pert);
            }
            bool success = false;
            for (auto pert : perturbations) { // Evaluate perturbations
                double afv = af.Evaluate(gp, pert, target);
                if (afv > best_afv_run) { // Move
                    best_afv_run = afv;
                    target_run = gp->f(pert.data());
                    best_point_run = pert;
                    success = true;
                }
            }
            if (success) {
                continue;
            } else { // Contract pattern
                for (int i = 0; i < step_lengths_.size(); ++i) {
                    if (step_lengths_(i) > min_step_lengths_(i)) {
                        step_lengths_(i) = step_lengths_(i) / 2;
                    }
                }
            }
            bool converged = true;
            for (int j = 0; j < step_lengths_.size(); ++j) { // Check if converged
                if (step_lengths_(j) > min_step_lengths_(j)) {
                    converged = false;
                    break;
                }
            }
            if (converged) {
                break;
            }
        }
        if (best_afv_run > best_afv) {
            best_afv = best_afv_run;
            best_point = best_point_run;
        }
    }
//    cout << "CS -- afv: " << best_afv << " ; ev: " << target << endl;
    return best_point;
}
VectorXd AFCompassSearch::generateRandomVector() {
    VectorXd rands = VectorXd::Zero(lb_.size());
    for (int i = 0; i < lb_.size(); ++i) {
        rands(i) = random_double(gen_, lb_(i), ub_(i));
    }
    return rands;
}

}
}
}
}
