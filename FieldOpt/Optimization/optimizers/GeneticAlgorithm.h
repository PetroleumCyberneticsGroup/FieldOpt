/******************************************************************************
   Created by einar on 2/27/17.
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
#ifndef FIELDOPT_GENETICALGORITHM_H
#define FIELDOPT_GENETICALGORITHM_H

#include <boost/random.hpp>
#include "optimizer.h"

using namespace std;

namespace Optimization {
namespace Optimizers {

/*!
 * @brief The GeneticAlgorithm class is an implentation of the MI-LXPM algorithm
 * described in "A real coded genetic algorithm for solving integer and mixed integer
 * optimization problems" -- Deep, K. (2009).
 *
 * The mutation operator used is Power Mutation; the crossover operator is Laplace
 * Crossover; and the selection operator is Tournament Selection.
 *
 * \note The Power Mutation _requires_ upper and lower bounds on the variables.
 * This class does not support any other forms of constraints.
 *
 * \note The number of chromosomes must be an even number.
 *
 * \note The tournament size is fixed at two.
 *
 * \note Elitism is enforced. The best individual from the last generation is kept.
 */
class GeneticAlgorithm : public Optimizer {
 public:
  GeneticAlgorithm(Settings::Optimizer *settings,
                   Case *base_case,
                   Model::Properties::VariablePropertyContainer *variables,
                   Reservoir::Grid::Grid *grid);
  TerminationCondition IsFinished() override;
 protected:
  void handleEvaluatedCase(Case *c) override;
  void iterate() override;
 private:
  boost::random::mt19937 gen_; // Random number generator with the random functions in math.hpp

  /*!
   * @brief The Chromosome struct is used to hold variable values
   * for a chromosome. It is used in place of cases because for
   * the mixed-integer algorithm implemented here, the integer variables
   * should be treated as floating point numbers until they are truncated
   * at the very end.
   */
  struct Chromosome {
    Eigen::VectorXd int_vars;
    Eigen::VectorXd rea_vars;
    Case *case_pointer;
    Chromosome(Case *c);
    Chromosome() {}
    double ofv() { return case_pointer->objective_function_value(); }
    void createNewCase();
   private:
    void truncateInts();
  };

  vector<Chromosome> population_; //!< Holds the set of cases currently used as chromomes by the algorithm.
  int n_ints_; //!< Number of integer variables.
  int n_reas_; //!< Number of real variables.

  // Parameters for genetic algorithm in general.
  int max_generations_; //!< Maximum number of generations.
  int population_size_; //!< Size of population (number of chromosomes).
  float p_crossover_; //!< Crossover probability.
  float p_mutation_; //!< Mutation probability.

  // Parameters from Laplace Crossover.
  float location_parameter_; //!< The location parameter used in the laplace distribution.
  float scale_real_; //!< Scale parameter for real numbers. Higher values give a larger probability of offspring further from parents.
  int scale_int_; //!< Scale parameter for integer numbers. Higher values give a larger probability of offspring further from parents.

  // Parameters for Power Mutation
  float power_real_; //!< Strength of real power mutation.
  float power_int_; //!< Strength of integer power mutation.

  // Bound constraints for variables
  Eigen::VectorXd rea_lower_bounds_; //!< Lower bounds for real numbers.
  Eigen::VectorXd rea_upper_bounds_; //!< Upper bounds for real numbers.
  Eigen::VectorXd int_lower_bounds_; //!< Lower bounds for integer numbers.
  Eigen::VectorXd int_upper_bounds_; //!< Upper bounds for integer numbers.

  /*!
   * @brief Sort the population according to descending objective function values.
   *
   * This is used to ease the enforcement of elitism.
   */
  void sortPopulation();

  /*!
   * @brief Perform tournament selection on the population. The tournament is fixed at two.
   *
   * The first place in the mating pool is reserved for the best individual from
   * the last generation (elitism).
   */
  void tournamentSelection();

  /*!
   * @brief Perform Laplace crossover on the population.
   *
   * The first individual in the list is not touched, in order to enforce elitism.
   */
  void laplaceCrossover();

  /*!
   * @brief Perform power mutation on the population.
   *
   * The first individuals in the list is not touched, in order to enforce elitism.
   */
  void powerMutation();

  /*!
   * @brief Print a string representation of the population to stdout.
   */
  void printPopulation();
};

}
}

#endif //FIELDOPT_GENETICALGORITHM_H
