/***********************************************************
 Copyright (C) 2017
 Mathias C. Bellout <mathias.bellout@ntnu.no>

 Created by bellout on 20180621.

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

// ---------------------------------------------------------
#ifndef FIELDOPT_RES_XYZ_REGION_H
#define FIELDOPT_RES_XYZ_REGION_H


// ---------------------------------------------------------
#include "constraint.h"
#include "constraint_wspln.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
class ResXYZRegion : public Constraint, WellSplineConstraint
{
 public:
  // -------------------------------------------------------
  ResXYZRegion(
      ::Settings::Optimizer::Constraint settings,
      ::Model::Properties::VariablePropertyContainer *variables);
  string name() override { return "res_xyz_region"; };

  // -------------------------------------------------------
  // Constraint interface
  bool CaseSatisfiesConstraint(Case *c);
  void SnapCaseToConstraints(Case *c);

 private:
  // -------------------------------------------------------
  double distance_;
  Well affected_well_;
  QList<Well> affected_wells_;

};


}
}

#endif //FIELDOPT_RES_XYZ_REGION_H
