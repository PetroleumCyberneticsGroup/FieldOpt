/***********************************************************
 Copyright (C) 2017
 Mathias C. Bellout <mathias.bellout@ntnu.no>

 Created by bellout on ${myDate}.

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

#include "res_xyz_region.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
ResXYZRegion::ResXYZRegion(
    ::Settings::Optimizer::Constraint settings,
    ::Model::Properties::VariablePropertyContainer *variables){

  // -------------------------------------------------------
  for (QString name : settings.wells) {

    // -----------------------------------------------------
    // Build affected well QList
    affected_wells_.append(
        initializeWell(variables->GetWellSplineVariables(name))
    );

  }

};

// =========================================================


}
}



