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
 * Note that the Power Mutation _requires_ upper and lower bounds on the variables.
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
  // Parameters for genetic algorithm in general.
  int max_generations_; //!< Maximum number of generations.
  int population_size_; //!< Size of population (number of chromosomes).
  double p_crossover_; //!< Crossover probability.
  double p_mutation_; //!< Mutation probability.

  // Parameters from Laplace Crossover.
  double location_parameter_; //!< The location parameter used in the laplace distribution.
  double scale_real_; //!< Scale parameter for real numbers. Higher values give a larger probability of offspring further from parents.
  int scale_int_; //!< Scale parameter for integer numbers. Higher values give a larger probability of offspring further from parents.

  // Parameters for Power Mutation
  double power_real_; //!< Strength of real power mutation.
  double power_int_; //!< Strength of integer power mutation.

  // Bound constraints for variables
  std::vector<double> float_lower_bounds_; //!< Lower bounds for real numbers.
  std::vector<double> float_upper_bounds_; //!< Upper bounds for real numbers.
  std::vector<int> int_lower_bounds_; //!< Lower bounds for integer numbers.
  std::vector<int> int_upper_bounds_; //!< Upper bounds for integer numbers.
};

}
}

#endif //FIELDOPT_GENETICALGORITHM_H
