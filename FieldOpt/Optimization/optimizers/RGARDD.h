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

class RGARDD : public GeneticAlgorithm {
 public:
  RGARDD(Settings::Optimizer *settings,
         Case *base_case,
         Model::Properties::VariablePropertyContainer *variables,
         Reservoir::Grid::Grid *grid);
 private:
  vector<Chromosome> mating_pool_;
  bool is_stagnant();
  void repopulate();
  double discard_parameter_;
  double stagnation_limit_;
  void iterate() override;
  vector<Chromosome> selection(vector<Chromosome> population) override;
  void handleEvaluatedCase(Optimization::Case *c) override;
  vector<Chromosome> crossover(vector<Chromosome> mating_pool) override;
  vector<Chromosome> mutate(vector<Chromosome> offspring) override;

};

}
}

#endif //FIELDOPT_RGARDD_H
