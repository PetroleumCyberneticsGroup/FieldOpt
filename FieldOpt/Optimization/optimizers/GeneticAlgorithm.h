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
 * @brief The abstract GeneticAlgorithm class a specification of the
 * structure for a general genetic algorithm. Crossover, mutation and
 * selection operators must be specified in a child class.
 */
class GeneticAlgorithm : public Optimizer {
 public:
  GeneticAlgorithm(Settings::Optimizer *settings,
                   Case *base_case,
                   Model::Properties::VariablePropertyContainer *variables,
                   Reservoir::Grid::Grid *grid);
  TerminationCondition IsFinished() override;
 protected:
  virtual void handleEvaluatedCase(Case *c) = 0;
  virtual void iterate() = 0;
 protected:
  boost::random::mt19937 gen_; // Random number generator with the random functions in math.hpp

  /*!
   * @brief The Chromosome struct is used to hold variable values
   * for a chromosome. It is used in place of cases because for
   * the mixed-integer algorithm implemented here, the integer variables
   * should be treated as floating point numbers until they are truncated
   * at the very end.
   */
  struct Chromosome {
    Eigen::VectorXd rea_vars;
    Case *case_pointer;
    Chromosome(Case *c);
    Chromosome() {}
    double ofv() { return case_pointer->objective_function_value(); }
    void createNewCase();
  };

  vector<Chromosome> population_; //!< Holds the current population.

  int max_generations_; //!< Maximum number of generations.
  int population_size_; //!< Size of population (number of chromosomes).
  double p_crossover_; //!< Crossover probability.
  double p_mutation_; //!< Mutation probability.
  double decay_rate_;
  double mutation_strength_;

  /*!
   * @brief Perform selection on the population.
   * @param population The population to perform selection on.
   * @return A vector containing the mating pool.
   */
  virtual vector<Chromosome> selection(vector<Chromosome> population) = 0;

  /*!
   * @brief Perform crossover on the mating pool.
   * @param mating_pool The mating pool to perform crossover on.
   * @return A vector containing offspring.
   */
  virtual vector<Chromosome> crossover(vector<Chromosome> mating_pool) = 0;

  /*!
   * @brief Perform mutation on the offspring.
   * @param offspring The individuals to perform mutation on.
   * @return Mutated individuals.
   */
  virtual vector<Chromosome> mutate(vector<Chromosome> offspring) = 0;

  /*!
   * @brief Sort the population according to fitness
   */
  vector<Chromosome> sortPopulation(vector<Chromosome> population);

  /*!
   * @brief Print a string representation of the population to stdout.
   */
  void printPopulation();

  void printChromosome(Chromosome &chrom);
};

}
}

#endif //FIELDOPT_GENETICALGORITHM_H
