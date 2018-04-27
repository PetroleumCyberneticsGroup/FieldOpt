/******************************************************************************
   Created by einar on 6/2/17.
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
#include "AcquisitionFunction.h"
#include <stdio.h>
#include "gp/gp_utils.h"
#include <math.h>

namespace Optimization {
namespace Optimizers {
namespace BayesianOptimization {

AcquisitionFunction::AcquisitionFunction() {
    weight_ev_ = 0.95;
    weight_var_ = 0.05;
}

AcquisitionFunction::AcquisitionFunction(Settings::Optimizer::Parameters settings) {
    weight_ev_ = 0.95;
    weight_var_ = 0.05;
}
double AcquisitionFunction::Evaluate(libgp::GaussianProcess *gp, Eigen::VectorXd x, double target) {
    double g = (gp->f(x.data()) - target) / sqrt(gp->var(x.data()));
//    double ei = ( gp->f(x.data()) - target) * libgp::Utils::cdf_norm(g)
//        + sqrt(gp->var(x.data())) * 1/(2*M_PI)*exp(-1*(g*g)/2);
    double ei = sqrt(gp->var(x.data()))
        * (g * libgp::Utils::cdf_norm(g)
            + 1.0/(2*M_PI) * exp(-0.5*g*g)
        );
    return ei;
}

}
}
}
