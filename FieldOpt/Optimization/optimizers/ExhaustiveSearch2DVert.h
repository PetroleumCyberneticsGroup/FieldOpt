/******************************************************************************
   Copyright (C) 2016 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#ifndef FIELDOPT_EXHAUSTIVESEARCH2DVERT_H
#define FIELDOPT_EXHAUSTIVESEARCH2DVERT_H

#include "Optimization/optimizer.h"

namespace Optimization {
namespace Optimizers {

/*!
 * @brief This ExhaustiveSearch algorithm is intended for doing an exhaustive search
 * (i.e. check all possible permutations) for the placement of _one_ verical well
 * in two dimensions. It works by creating Case objects for all combinations of the
 * i and j variables that are inside the reservoir.
 */
class ExhaustiveSearch2DVert : public Optimizer {
 public:
  /*!
   * @brief Default constructor. Checks that the problem has exactly three variables: i,
   * j and k coordinates for a well block. Also retrieves the UUIDs for the i and j variables.
   */
  ExhaustiveSearch2DVert(Settings::Optimizer *settings,
                         Case *base_case,
                         Model::Properties::VariablePropertyContainer *variables,
                         Reservoir::Grid::Grid *grid,
                         Logger *logger
  );
 private:
  Reservoir::Grid::Grid *grid_;
  QUuid i_varid;
  QUuid j_varid;

  /*!
   * @brief This will return NOT_FINISHED if either iterate() has not been called yet,
   * or if all cases have not yet been evaluated.
   * @return
   */
  virtual TerminationCondition IsFinished() override;

  /*!
   * @brief Creates a Case for every permutation of the i and j variables that are inside
   * the reservoir the first time its called. Calling it more than once has no effect.
   */
  virtual void iterate() override;

protected:
    void handleEvaluatedCase(Case *c) override {}

};

}
}

#endif //FIELDOPT_EXHAUSTIVESEARCH2DVERT_H
