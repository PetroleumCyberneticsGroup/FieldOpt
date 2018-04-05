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
#ifndef FIELDOPT_ACQUISITIONFUNCTION_H
#define FIELDOPT_ACQUISITIONFUNCTION_H

#include <Settings/optimizer.h>
#include <libgp/include/gp.h>
namespace Optimization {
namespace Optimizers {
namespace BayesianOptimization {

/*!
 * @brief This class is an implementation of an AcquisitionFunction. It is
 * used in Bayesian Optimization algorithms to determine which point should
 * be evaluated next.
 *
 * It takes as input coordinate vector x, and returns a weighted sum of the
 * expected value at x and the variance at x.
 *
 * Weighing the expected value component higher will result in faster convergence
 * and less exploration (thus increasing the risk that we will miss the global
 * optimum); while wighing the variance component higher will result in more
 * exploration (this giving slower convergence and reducing the risk that we
 * will miss the true global optimum).
 */
class AcquisitionFunction {
 public:
  /*!
   * @brief Initialize the acquisition function, setting the weight parameters.
   */
  AcquisitionFunction(Settings::Optimizer::Parameters settings);
  AcquisitionFunction();

  /*!
   * @brief Evaluate the AcquisitionFunction at a point, returning the weighed sum
   * of the expected value and the variance at the point according to the Gaussian
   * Process instance.
   * @param gp Gaussian process to infer from.
   * @param x Coordinate to be evaluated.
   * @param target Incumbet target; usually the best observed value.
   * @return The Acquisition function value at the point x according to the GP.
   */
  double Evaluate(libgp::GaussianProcess *gp, Eigen::VectorXd x, double target=0);

 private:
  double weight_ev_; //!< Expected value weight. (default: 0.5)
  double weight_var_; //!< Variance weight. (default: 0.5)
};

}
}
}
#endif //FIELDOPT_ACQUISITIONFUNCTION_H
