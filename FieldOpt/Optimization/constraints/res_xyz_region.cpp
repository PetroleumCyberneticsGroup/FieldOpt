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

// ---------------------------------------------------------
#include "res_xyz_region.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
ResXYZRegion::ResXYZRegion(
    ::Settings::Optimizer::Constraint settings,
    ::Model::Properties::VariablePropertyContainer *variables,
    ::Reservoir::Grid::Grid *grid,
    RICaseData *ricasedata){

  // -------------------------------------------------------
  settings_ = settings;
  variables_ = variables;
  grid_ = grid;
  ricasedata_ = ricasedata;

  // -------------------------------------------------------
  for (QString name : settings.wells) {

    // -----------------------------------------------------
    // Build affected well QList
    affected_wells_.append(
        initializeWell(variables->GetWellSplineVariables(name))
    );

  }


//  for (int i = 0; i < settings.poly_points.size(); i++) {
//
//    polypoints_.push_back(settings.poly_points.);
//  }


  // [1] --> OK
  // Introduce region points into
  // ::Settings::Optimizer::Constraint settings

  // [2]
  // Convert region points into poly-boundary

  // [3]
  // Print poly-boundary vertices to file

  // [4]
  // Use poly-boundary vertices in constriant-handling

};

// =========================================================
//bool ResXYZRegion::AssemblePolygon(){
//
//}


// =========================================================
bool ResXYZRegion::CaseSatisfiesConstraint(Case *c) {

  // -------------------------------------------------------
  QList<Eigen::Vector3d> points;
  vectorizeWells(c, affected_wells_, points);

}

// =========================================================
void ResXYZRegion::SnapCaseToConstraints(Case *c) {

  // -------------------------------------------------------
  QList<Eigen::Vector3d> points;
  vectorizeWells(c, affected_wells_, points);

}




}
}



