/******************************************************************************
   Copyright (C) 2015-2016 Mathias C. Bellout <mathias.bellout@ntnu.no>

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

#ifndef FIELDOPT_EXHAUSTIVESEARCH2DHORZ_H
#define FIELDOPT_EXHAUSTIVESEARCH2DHORZ_H

#include "Optimization/optimizer.h"

namespace Optimization {
namespace Optimizers {

/*!
 * @brief Performs approximate exhaustive search, i.e., it performs
 * a grid-type sampling that covers the entire solution space of
 * possible 2D positions of a singe horizontal well.
 *
 * Case objects for all combinations are created and launched in
 * parallel.
 */

class ExhaustiveSearch2DHorz : public Optimizer  {
 public:
  /*!
   * @brief Default constructor.
   * Checks that the problem has six variables altogether:
   * (x,y,z)_heel (x,y,z)_toe
   * Retrieves the UUIDs for the x, y, z heel and toe variables
   *
   *
   */

  ExhaustiveSearch2DHorz(Settings::Optimizer *settings,
                         Case *base_case,
                         Model::Properties::VariablePropertyContainer *variables,
                         Reservoir::Grid::Grid *grid);

 private:
  Reservoir::Grid::Grid *grid_;
  QList<QUuid> xyz_heel_varid;
  QList<QUuid> xyz_toe_varid;

  /*!
   * @brief Returns NOT_FINISHED if either iterate() has
   * not been called yet, or if all cases have not yet
   * been evaluated.
   * @return
   */
  virtual TerminationCondition IsFinished() override;

  /*!
   * @brief Creates a Case for every permutation of the i and j
   * variables that are inside the reservoir the first time its
   * called. Calling it more than once has no effect.
   */
  virtual void iterate() override;

 protected:
  void handleEvaluatedCase(Case *c) override {}

};

}
}

#endif //FIELDOPT_EXHAUSTIVESEARCH2DHORZ_H
