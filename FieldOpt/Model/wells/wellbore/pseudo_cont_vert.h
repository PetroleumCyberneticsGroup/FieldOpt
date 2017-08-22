/******************************************************************************
   Created by einar on 8/16/17.
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
#ifndef FIELDOPT_PSEUDOCONTVERT_H
#define FIELDOPT_PSEUDOCONTVERT_H

#include "Settings/model.h"
#include "Reservoir/grid/grid.h"
#include "Model/properties/variable_property_container.h"
#include "wellblock.h"

namespace Model {
namespace Wells {
namespace Wellbore {

/*!
 * @brief This class holds a representation of a vertical well that can be placed with
 * "pseudo-continuous" variables. By this we mean that the position is determined by
 * continuous variables in the xyz coordinate-system in the reservoir, but they are
 * converted into IJK indices and the well indices are defaulted when they are passed
 * to the simulator. Put simply, this implies that all wells will be assumed to be at
 * the center of the block that contains the xyz-coordinate, and that they fully
 * penetrate that block.
 *
 * For now, this class only considers x and y coordinates, and it assumes that only
 * one block (the topmost) is penetrated in the vertical direction. This means that
 * it's best suited for 2D reservoirs.
 *
 * Note that this class uses the depth of cell 1,1,1 as the z-coordinate when getting
 * the cell containing the xy-point, so reservoirs that don't have a flat top layer
 * will be problematic.
 */
class PseudoContVert {
 public:
  PseudoContVert(Settings::Model::Well well_settings,
                 Properties::VariablePropertyContainer *variable_container,
                 Reservoir::Grid::Grid *grid);

  /*!
   * @brief Get the well block. The transmissibility factor is defaulted (i.e. set negative).
   */
  WellBlock *GetWellBlock();

 private:
  Reservoir::Grid::Grid *grid_;
  double z_pos_;
  Properties::ContinousProperty *x_pos_;
  Properties::ContinousProperty *y_pos_;
};

}
}
}

#endif //FIELDOPT_PSEUDOCONTVERT_H
