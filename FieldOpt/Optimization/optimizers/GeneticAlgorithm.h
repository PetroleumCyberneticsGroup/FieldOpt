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
    Case *c;
    Chromosome(Case *c);
    Chromosome() {}
    void createNewCase();
   private:
    void truncateInts();
  };

  vector<Chromosome> population_; //!< Holds the set of cases currently used as chromomes by the algorithm.
  int n_ints_; //!< Number of integer variables.
  int n_floats_; //!< Number of real variables.

  // Parameters for genetic algorithm in general.
  int max_generations_; //!< Maximum number of generations.
  int generation_; //!< The curent interation number.
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
  Eigen::VectorXd float_lower_bounds_; //!< Lower bounds for real numbers.
  Eigen::VectorXd float_upper_bounds_; //!< Upper bounds for real numbers.
  Eigen::VectorXd int_lower_bounds_; //!< Lower bounds for integer numbers.
  Eigen::VectorXd int_upper_bounds_; //!< Upper bounds for integer numbers.

  /*!
   * @brief Perform tournament selection on the population.
   * @return The set of cases (chromomes) to be used mating pool.
   */
  vector<Chromosome> tournamentSelection();

  /*!
   * @brief Perform Laplace crossover on the members of the mating pool. This creates a new set of cases.
   * @param mating_pool The list of cases to be used as a mating pool.
   * @return A list of new cases resulting from the crossover.
   */
  vector<Chromosome> laplaceCrossover(vector<Chromosome> mating_pool);

  /*!
   * @brief Perform power mutation on the list of chromosomes.
   * @param chromosomes The chromosomes to be mutated.
   * @return A list of mutated chromosomes.
   */
  vector<Chromosome> powerMutation(vector<Chromosome> chromosomes);
};

}
}

#endif //FIELDOPT_GENETICALGORITHM_H
