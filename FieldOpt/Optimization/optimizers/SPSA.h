/******************************************************************************
   Created by einar on Jan. 14. 2019
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#ifndef FIELDOPT_SPSA_H
#define FIELDOPT_SPSA_H

#include "Optimization/optimizer.h"
#include "Utilities/math.hpp"
#include <Eigen/Core>
#include <utility>

namespace Optimization {
namespace Optimizers {

/*!
 * @brief This class implements the Simultaneous Perturbation Stochastic Approximation (SPSA)
 * algorithm.
 *
 * The implementation is based on the description in
 * [1] "An overview of the simultaneous perturbation method for efficient optimization", 
 *	James C. Spall, Johns Hopkins Techical Digest, vol. 19 no. 4 (1998).
 *
 * The default parameter values are based on the recommendations described in
 * [2] "Implementation of the simultaneous perturbation algorithm for stochastic optimization",
 *	James C. Spall, IEEE Transactions on Aearospace and Electronic Systems, vol. 34 no. 3 (1998)
 */
class SPSA : public Optimizer {
 public:
  SPSA(Settings::Optimizer *settings,
	   Case *base_case,
	   Model::Properties::VariablePropertyContainer *variables,
	   Reservoir::Grid::Grid *grid,
	   Logger *logger,
	   CaseHandler *case_handler=0,
	   Constraints::ConstraintHandler *constraint_handler=0);

  TerminationCondition IsFinished() override;

 protected:
  void handleEvaluatedCase(Case *c) override;
  void iterate() override;

 private:

  // Parameters
  int max_iterations_; //!< Maximum number of iterations allowed.
  double alpha_; //!< Primarily used to account for noise. Default: 0.602.
  double gamma_; //!< Primarily used to account for noise. Default: 0.101.
  double a_;     //!< Main-parameter for step length scaling.
  double A_;     //!< "Stability" constant useful in controlling step lengths in early iterations.
  double c_;     //!< Used to compensate for noise.
  double init_step_magnitude_; //!< Used to compute the a_ parameter.

  // Run-time variables
  bool perturbations_evaluated_; //!< Indicates whether the perturbations used for grad. est. have been evaluated.
  int D_;        //!< Dimensionality of problem.
  boost::random::mt19937 gen_; //!< Random number generator.
  double a_k_;             //!< Gain sequence a at iteration k: \$ a_k = a/(A+k)^{\alpha} \$.
  double c_k_;    //!< Gain sequence c at iteration k: \$ c_k = c/k^{\gamma} \$
  Eigen::VectorXd g_k_;    //!< Gradient at iteration k.
  Eigen::VectorXd delta_k_; //!< Simultaneous perturbation vector at iteration k.

  std::pair<Case *, Case *> perturbations_; //!< Perturbations currently used for gradient computation.
  Case *estimate_; //!< The estimated best case. This is never actually evaluated.

  /*!
   * Indicates whether the perturbations generated are valid.
   * If they are not, the algorithm will attempt to generate new perturbations
   * until a valid pair is found.
   */
  bool perturbations_valid_;

  /*!
   * @brief Update the \$ a_k \$ gain parameter.
   *
   * \$ a_k = a / (A+k)^{\alpha} \$
   */
  void update_a_k();

  /*!
   * @brief Update the \$ c_k \$ gain parameter.
   *
   * \$ c_k = c / k^{\gamma} \$
   */
  void update_c_k();

  /*!
   * @brief Update the simultaneous perturbation vector delta_k_
   *
   * It is set to a vector with D_ random elements, each on either +1 or -1 with 0.5 probability.
   */
  void updateSPVector();

  /*!
   * @brief Generate two perturbations to be used for gradient calculation:
   * \$ \theta_k + c_k \Delta_k \$ and \$ \theta_k - c_k \Delta_k \$ where
   * \$ \theta_k \$ is the tentative best case.
   */
  std::pair<Case *, Case *> createPerturbations();

  /*!
   * @brief Update g_k_ by using the two cases in perturbations_.
   * The gradient is updated according to:
   * \$ g_k (\theta_k) = \frac{y(\theta_k + c_k \Delta_k) 
   *                     - y(\theta_k - c_k \Delta_k}{2c_k} 
   *                     * [\Delta^{-1}_{k1}, \Delta^{-1}_{k2}, ... , \Delta^{-1}_{kD} ]^T \$
   */
  void updateGradient();

  /*!
   * @brief Update the estimate_ field (estimated best position).
   * \$ \theta_{k+1} = \theta_k - a_k g_k(\theta_k) \$
   *
   * If no new tentative best case has been found in the 0.2*max_iterations_ iterations,
   * the estimate will be set to the tentative best case.
   */
  void updateEstimate();

  /*!
   * Compute the a_ parameter using init_step_magnitude_  after evaluation of the first pair of
   * perturbations. Only called if init_step_magnitude_ != 0.0.
   */
  void compute_a();
};

}
}

#endif
