/******************************************************************************
   Created by einar on 1/10/19.
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
#ifndef FIELDOPT_VFSA_H
#define FIELDOPT_VFSA_H

#include "Optimization/optimizer.h"
#include "Utilities/math.hpp"
#include "Utilities/random.hpp"
#include <Eigen/Core>

namespace Optimization {
namespace Optimizers {

/*!
 * @brief This class implements the Very Fast Simulated Annealing (VFSA) optimization algorithm.
 *
 * The implementation is based on the descriptions in
 *      "Very fast simulated re-annealing", L. Ingber, Mathematical and Computer Modelling (1989)
 * and
 *      "FVSARES---a very fast simulated annealing FORTRAN program for interpretation of 1-D DC resistivity
 *      sounding data from various electrode arrays", Shashi Prakash Sharma, Computers & Geosciences (2012).
 *
 * Note that this algorithm requires boundary constraints to be specified.
 */
class VFSA : public Optimizer {
 public:
  VFSA(Settings::Optimizer *settings,
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

  /*!
   * @brief Calculate the selection probability. Used to determine whether a new case will be set as the
   * tentative best case.
   *
   * \$ h(x) = exp( - \Delta E / T ) \$, where \$ \Delta E \$ is the difference between the new objective
   * function value and the tentative best.
   * @param old_ofv The tentative best objective function value.
   * @param new_ofv The new objective function to check.
   * @return h(x)
   */
  double selectionProbability(const double old_ofv, const double new_ofv) const;

  /*!
   * @brief Compute the next temperature vector.
   *
   * \$ T_i(k) = T_{0,i} exp( -c_i k^{1/D} \$ where \$ k \$ is the current iteration number and \$ T_{0,i} \$ is
   * the current (old) temperature for variable \$ i \$.
   * @param old_T Old temperature vector.
   * @return The new temperature vector.
   */
  Eigen::VectorXd nextTemperature(const Eigen::VectorXd old_T) const;

  /*!
   * @brief Compute the updating factors \$ y_i \$ for all variables.
   *
   * \$ y_i = sgn(u_i - 0.5) T_i \left[ (1+ 1/T_i)^{|2u_i+1|} - 1 \right] \$ where \$ sgn() \$ is the sign function
   * and \$ u_i \$ is a uniformly generated random number in the range [-1,1].
   * @param T Vector of all temperatures.
   * @return Vector of all updating factors \$ y_i \$.
   */
  Eigen::VectorXd updatingFactors(const Eigen::VectorXd T);

  /*!
   * @brief Create a new perturbation.
   *
   * \$ x_{k+1} = x_k + y * [x_{max} - x_{min}] \$
   *
   * @return
   */
  Case *createPerturbation();

  bool parallel_;              //!< Use parallel mode. If true, evals_pr_iteration_ cases will be generated immediately when a new iteration starts.
  int evals_pr_iteration_;     //!< Number of evaluations (perturbations) at each temperature level (iteration).
  int max_iterations_;         //!< Maximum number of iterations.
  int D_;                      //!< Dimensionality (number of variables in the problem).

  int evals_in_iteration_;     //!< Number of evaluations that have been performed in the current iteration.

  boost::random::mt19937 gen_; //!< Random number generator.
  Eigen::VectorXd T_;          //!< Vector containing current temperatures (one element pr. dimension).
  Eigen::VectorXd min_;        //!< Vector containing lower bounds for variables.
  Eigen::VectorXd max_;        //!< Vector containing upper bounds for variables.
  Eigen::VectorXd c_;          //!< Constant used in updating the temperature.
};

}
}

#endif //FIELDOPT_VFSA_H
