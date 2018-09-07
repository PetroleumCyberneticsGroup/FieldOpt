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
#ifndef FIELDOPT_AFPSO_H
#define FIELDOPT_AFPSO_H

#include <boost/random.hpp>
#include "AFOptimizer.h"
#include <vector>

using namespace Eigen;
using namespace std;

namespace Optimization {
namespace Optimizers {
namespace BayesianOptimization {
namespace AFOptimizers {

/*!
 * @brief The AFPSO class implements a Particle Swarm Optimization algorithm
 * for the Acquisition Function.
 *
 * The implementation is based on the description in
 *  Nwankwor, Nagar & Reid (2012). "Hybrid differential evolution and particle
 *  swarm optimization for optimal well placement", Computational Geosciences.
 *
 *  A static ring topology is used for neighbourhoods. The array of p=6 particles
 *  [P1, P2, P3, P4, P5, P6]
 *  can be visialized in a ring topology with neighbourhood size N=3 as
 *
 *  P1 - P2   P4 - P5
 *   \P3/      \P6/
 *
 * I.e. Each particle in a neighbourhood is connected to the neighbouring particles,
 * and the first and last particles in the neighbourhood are connected to each other.
 */
class AFPSO : public AFOptimizer {

 public:
  Eigen::VectorXd Optimize(libgp::GaussianProcess *gp, AcquisitionFunction &af, double target) override;

  AFPSO();

  /*!
   * @brief Initialize the optimizer, setting the bounds for the variables.
   * @param lb Lower bounds.
   * @param ub Upper bounds.
   * @rng_seed Seed to use for the random number generator.
   */
  AFPSO(VectorXd lb, VectorXd ub, int rng_seed=0);

 private:
  struct Particle {
    VectorXd vel; //!< Velocity of the particle at iteration k. (v)
    VectorXd pos; //!< Position of the particle at iteration k. (x)
    VectorXd pos_best_self; //!< Best position found by particle up to iteration k. (y)
    VectorXd pos_best_nbhd; //!< Best position found by any particle in the neighbourhood up to iteration k. (y^*)
    double fit; //!< The computed fitness for the particle at the current position (f(x))
    double fit_best_self; //!< The best fitness found by this particle;
    double fit_best_nbhd; //!< The best fitness found in the neighbourhood of this particle.
    int nbr_1_idx; //!< Index of first neighbour.
    int nbr_2_idx; //!< Index of second neighbour.

    /*!
     * @brief Initialize the particle to a random position bounded by the lower and upper bounds.
     */
    Particle(VectorXd &lb, VectorXd &ub, boost::mt19937 &gen);

    /*!
     * @brief Update the velocity of this particle according to
     * \f[
     *  v(k+1)_i = \omega v(k)_i
     *              + c_1 r_{1,i} (y_i(k) - x_i(k))
     *              + c_2 r_{2,i} (y^*_i(k) - x_i(k))
     * \f]
     * for each dimension i, where \f$ r_{1/2, i} \f$ are random numbers from a uniform
     * distribution [0, 1], different for each dimensional compontent.
     */
    void update_velocity(double intertia, double c1, double c2, boost::random::mt19937 &gen);

    /*!
     * @brief Update the position of this particle according to
     * \f[
     *  x(k+1) = x(k) + v(k+1)
     * \f]
     */
    void update_position(VectorXd &lb, VectorXd &ub);

   private:
    /*!
     * @brief Check if any bounadry constraints are broken. If they are,
     * apply absorption (set the value to the bound value if it is broken,
     * and set the velocity along the dimension to zero).
     */
    void check_boundaries(VectorXd &lb, VectorXd &ub);
  };

  int n_particles_; //!< Number of particles to be used.
  int n_neighbourhoods_; //!< Number of neighbourhoods to be used.
  int n_iters_; //!< Number of iterations to be performed.
  int n_dims_; //!< Number of dimensions in the problem.
  int n_iterations_; //!< Maximum number of iterations.
  int iteration_; //!< Iteration counter.
  vector<Particle> pop_;

  boost::random::mt19937 gen_; //!< Random number generator.
  double c1_; //!< Cognitive scaling parameter. (c_1)
  double c2_; //!< Social scaling parameter. (c_2)
  double inertia_; //!< Inertia term for the velocity update (omega)
  VectorXd lb_; //!< Lower bounds for the variables.
  VectorXd ub_; //!< Upper bounds for the variables.

};

}
}
}
}

#endif //FIELDOPT_AFPSO_H
