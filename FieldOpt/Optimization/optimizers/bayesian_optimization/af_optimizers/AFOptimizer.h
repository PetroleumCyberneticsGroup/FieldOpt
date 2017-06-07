/******************************************************************************
   Created by einar on 6/6/17.
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
#ifndef FIELDOPT_AFOPTIMIZER_H
#define FIELDOPT_AFOPTIMIZER_H

#include <Settings/optimizer.h>
#include <Eigen/Core>
#include <libgp/include/gp.h>
#include <optimizers/bayesian_optimization/AcquisitionFunction.h>
namespace Optimization {
namespace Optimizers {
namespace BayesianOptimization {
namespace AFOptimizers {

/*!
 * @brief The AFOptimizer (Acquisition Function Optimizer) class defines an interface
 * for acquisition function optimizers, i.e. optimizers for the acquisition function
 * that determines the next case to be evaluated by optimizing the expected value and
 * uncertainty in the gaussian process.
 */
class AFOptimizer {
 public:

  AFOptimizer();

  /*!
   * @brief Optimize the acquisiton function wrt. the gaussian process, returning one
   * optima.
   * @param gp Gaussion process model.
   * @param af Acquisition function.
   * @return One (local) optima for the acquisition function.
   */
  virtual Eigen::VectorXd Optimize(libgp::GaussianProcess *gp, AcquisitionFunction &af) = 0;

};

}
}
}
}

#endif //FIELDOPT_AFOPTIMIZER_H
