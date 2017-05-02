/******************************************************************************
   Created by einar on 3/27/17.
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
#ifndef FIELDOPT_RGARDD_H
#define FIELDOPT_RGARDD_H

#include "GeneticAlgorithm.h"

namespace Optimization {
namespace Optimizers {

/*!
 * @brief This implementation is based on the RGA-RDD (Real coded
 * Genetic Algorithm - Ranking selection, Direction-based crossover,
 * Dynamic random mutation) described in
 *
 *  "A real-coded genetic algorithm with a selection-based crossover
 *  operator", Chuang, Chen & Hwang,  Elsevier Journal of Information
 *  Sciences (2015).
 *
 * It uses the following operators:
 *  - Selection: Ranking Selection (RS)
 *  - Crossover: Direction-Based Crossover (DBX)
 *  - Mutation: Dynamic Random Mutation (DRM)
 *  - Replacement: Generational Replacement (GR) with elitism.
 *
 * Additionally, stagnation is alleviated by regenerating the population
 * (except for the best individual) whenever a stagnation indicator
 * passes below the stagnation limit. The stagnation indicator used
 * is the standard deviation of the population.
 *
 * \note This algorithm requires either that simple max/min bounds are
 * given (i.e. single numbers applying to all variables) as an optimizer
 * argument or that some form of bound constraints are used (e.g. reservoir
 * boundary or pressure/rate constraints).
 */
class RGARDD : public GeneticAlgorithm {
 public:
  RGARDD(Settings::Optimizer *settings,
         Case *base_case,
         Model::Properties::VariablePropertyContainer *variables,
         Reservoir::Grid::Grid *grid,
         Logger *logger
  );
 private:
  vector<Chromosome> mating_pool_; //!< Holds the current mating pool.
  double discard_parameter_; //!< Determines the fraction of parents to be discarded in selection.
  double stagnation_limit_; //!< The threshold for when to regenerate the population.

  /*!
   * @brief Perform the next iteration by generating a new mating pool
   * from the current population, or, if the population has stagnated,
   * repopulating it.
   */
  void iterate() override;

  /*!
   * @brief Handle an evaluated case.
   *
   * Replaces the parent chromosome in the population with the offspring
   * chromosome in the mating pool if it is better. Updates the tentative
   * best case if necessary.
   * @param c The newly evaluated case.
   */
  void handleEvaluatedCase(Optimization::Case *c) override;

  /*!
   * @brief Perform Ranking Selection on the population to generate a
   * new mating pool. Expects a population from best to worst fitness.
   * @param population The population to perform selection on.
   * @return A new mating pool.
   */
  vector<Chromosome> selection(vector<Chromosome> population) override;

  /*!
   * @brief Perform Direction-Based Crossover on two parents from the
   * mating pool.
   * @param mating_pool The two parents to be used.
   * @return Vector containing two new offspring.
   */
  vector<Chromosome> crossover(vector<Chromosome> mating_pool) override;

  /*!
   * @brief Perform Dynamic Random Mutation on two parents from the mating
   * pool.
   * @param mating_pool The two parents to be used.
   * @return Vector containing two new offspring.
   */
  vector<Chromosome> mutate(vector<Chromosome> mating_pool) override;

  /*!
   * @brief Check if the population has stagnated.
   *
   * This is done by calculating the standard deviation of a vector
   * containing the sum of the variable values for each chromosome
   * in the population and checking if it's below the stagnation
   * threshold.
   * @return True if the population is stagnant; otherwise false.
   */
  bool is_stagnant();

  /*!
   * @brief Replace all individuals except the best one (i.e. the
   * first element in the sorted population list) with new, random
   * chromosomes. Expects the population to be pre-sorted.
   */
  void repopulate();

  /*!
   * @brief Snap the variable values in a chromosome to the upper and lower bounds.
   * @param chrom The chromosome to be snapped.
   */
  void snap_to_bounds(Chromosome &chrom);

  class ConfigurationSummary : public Loggable {
   public:
    ConfigurationSummary(RGARDD *opt) { opt_ = opt; }
    LogTarget GetLogTarget() override;
    map<string, string> GetState() override;
    QUuid GetId() override;
    map<string, vector<double>> GetValues() override;
   private:
    RGARDD *opt_;
  };

};

}
}

#endif //FIELDOPT_RGARDD_H
