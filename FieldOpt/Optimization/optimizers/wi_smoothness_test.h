/******************************************************************************
   Copyright (C) 2017 Mathias C. Bellout <mathias.bellout@ntnu.no>

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

#ifndef FIELDOPT_WI_SMOOTHNESS_TEST_H
#define FIELDOPT_WI_SMOOTHNESS_TEST_H

#include "Optimization/optimizer.h"
#include "GSS.h"

namespace Optimization {
namespace Optimizers {

/*!
 * @brief study of cost function smoothness as a
 * function of well index (differing well paths)
 *
 */
class WISmooothnessTest : public Optimizer {
 public:
  WISmooothnessTest(Settings::Optimizer *settings,
                    Case *base_case,
                    Model::Properties::VariablePropertyContainer *variables,
                    Reservoir::Grid::Grid *grid,
                    Logger *logger);

 private:
  Reservoir::Grid::Grid *grid_;
  QUuid i_varid;
  QUuid j_varid;

  QString GetStatusStringHeader() const;
  QString GetStatusString() const;

  /*!
  * @brief
  * @return
  */
  void iterate();

};

} // namespace Optimization
} // namespace Optimizers

#endif //FIELDOPT_WI_SMOOTHNESS_TEST_H
