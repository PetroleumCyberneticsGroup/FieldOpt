/******************************************************************************
   Created by einar on 6/7/17.
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
#include "AFMonteCarlo.h"


namespace Optimization {
namespace Optimizers {
namespace BayesianOptimization {
namespace AFOptimizers {

AFMonteCarlo::AFMonteCarlo(VectorXd lb, VectorXd ub) {
    lb_ = lb;
    ub_ = ub;
    trials_ = 1000;
    gen_ = get_random_generator();
}
Eigen::VectorXd AFMonteCarlo::Optimize(libgp::GaussianProcess *gp, AcquisitionFunction &af, double target) {
    VectorXd best = generateRandomVector();
    double targ = target;
    for (int i = 0; i < trials_; ++i) {
        auto rand = generateRandomVector();
        double afv = af.Evaluate(gp, rand, targ);
        if (gp->f(rand.data()) > targ) {
            best = rand;
            targ = gp->f(rand.data());
        }
    }
    return best;
}
VectorXd AFMonteCarlo::generateRandomVector() {
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
