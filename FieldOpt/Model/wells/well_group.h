/***********************************************************
 Copyright (C) 2018-
 Mathias C. Bellout <mathias.bellout@ntnu.com>

 Created by bellout on 4/21/18.

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version
 3 of the License, or (at your option) any later version.

 FieldOpt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the
 GNU General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef FIELDOPT_WELL_GROUP_H
#define FIELDOPT_WELL_GROUP_H

// ---------------------------------------------------------
#include "Settings/settings.h"
#include "Settings/model.h"

#include "Reservoir/grid/eclgrid.h"
#include "Model/properties/variable_property_container.h"
#include "Model/properties/discrete_property.h"

#include "well.h"
#include "drilling_sequence.h"
// #include "Model/model.h"


// ---------------------------------------------------------
// STD
#include <string>
#include <vector>
#include <map>

// ---------------------------------------------------------
//namespace Model {
//struct Drilling{};
//}

// ---------------------------------------------------------
namespace Model {
namespace WellGroups {

// =========================================================
/*!
 * \brief Group class collects multiples wells in the model.
 */
class WellGroup {
 public:
  WellGroup(
      Settings::Model settings,
      int group_nr,
      ::Model::DrillingSequence drilling,
      ::Model::Properties::VariablePropertyContainer *variable_container,
      ::Reservoir::Grid::Grid *grid);

  // -------------------------------------------------------
  QList<Wells::Well *> *group_wells_;

  QList<Wells::Well *> *group_wells() const {
    return group_wells_;
  }

  // -------------------------------------------------------
  ::Model::Properties::DiscreteProperty *drill_seq_;

};

}
}

#endif //FIELDOPT_WELL_GROUP_H
