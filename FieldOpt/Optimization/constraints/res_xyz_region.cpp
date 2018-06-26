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
#include <FieldOpt-WellIndexCalculator/resinxx/rixx_prj_viz/RivIntersectionGeometryGenerator.h>
#include "res_xyz_region.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
ResXYZRegion::ResXYZRegion(
    ::Settings::Optimizer::Constraint &settings,
    ::Model::Properties::VariablePropertyContainer *variables,
    ::Reservoir::Grid::Grid *grid,
    RICaseData *ricasedata){

  // -------------------------------------------------------
  if (settings.verb_vector_[6] >= 1) // idx:6 -> opt
    cout << fstr("[opt]ResXYZRegion constraint.",6) << endl;

  // -------------------------------------------------------
  settings_ = settings;
  variables_ = variables;
  grid_ = grid;
  ricasedata_ = ricasedata;
  mod_offset_ = ricasedata_->mainGrid()->displayModelOffset();

  // -------------------------------------------------------
  for (QString name : settings.wells) {

    // -----------------------------------------------------
    // Build affected well QList
    affected_wells_.append(
        initializeWell(variables->GetWellSplineVariables(name))
    );

  }

  // [1] --> OK
  // Introduce region points into
  // ::Settings::Optimizer::Constraint settings
  polypoints_ = settings.poly_points;

  // [2]
  // Convert region points into poly-boundary
  AssembleRegion();

  // [3]
  // Print poly-boundary vertices to file
  PrintRegionVertices();

  // [4]
  // Use poly-boundary vertices in constriant-handling

};

// =========================================================
bool ResXYZRegion::AssembleRegion(){

  // -----------------------------------------------------
  rimintersection_ = new RimIntersection(ricasedata_->mainGrid(),
                                         ricasedata_,
                                         settings_);

  // -------------------------------------------------------
  if (settings_.verb_vector_[6] >= 1) // idx:6 -> opt
    cout << fstr("[opt]AssembleRegion().",6) << endl;

  // -----------------------------------------------------
  // Insert polypoints to rim intersection
  for (int i=0; i < polypoints_.size(); i++) {
    rimintersection_->appendPointToPolyLine(polypoints_[i]);
  }

  // -----------------------------------------------------
  // Run intersection part manager and geometry generator
  RivIntersectionPartMgr *imgr =
      rimintersection_->intersectionPartMgr();

  RivIntersectionGeometryGenerator
      *icsec = imgr->getCrossSectionGenerator();

  // -----------------------------------------------------
  // Extract vertices from geometry obj + vertex count
  regionvertices_ = icsec->m_cellBorderLineVxes;
  vx_count_ = regionvertices_.p()->size();

  for (size_t ivx = 0; ivx < vx_count_; ivx++) {

    cvf::Vec3f
        vx((float)(regionvertices_.p()->val(ivx).x() + mod_offset_.x()),
           (float)(regionvertices_.p()->val(ivx).y() + mod_offset_.y()),
           (float)(regionvertices_.p()->val(ivx).z() + mod_offset_.z()));

    regionvertices_.p()->set(ivx,vx);

  }

}

// =========================================================
bool ResXYZRegion::PrintRegionVertices(){

  // -------------------------------------------------------
  if (settings_.verb_vector_[6] >= 1) // idx:6 -> opt
    cout << fstr("[opt]PrintRegionVertices().",6) << endl;

  // -----------------------------------------------------
  // Print vertices to file
  string filename = settings_.output_dir_.toStdString() + "/REG_" +
      settings_.wells.join("_").toStdString() + ".PERIMETER";

  ofstream fperimeter(filename);
  fperimeter.precision(16);

  // -----------------------------------------------------
  for (size_t ivx = 0; ivx < vx_count_; ivx++) {
    fperimeter << regionvertices_.p()->val(ivx).x() << " "
               << regionvertices_.p()->val(ivx).y() << " "
               << regionvertices_.p()->val(ivx).z() << " "
               << endl;
  }

  fperimeter.close();

}

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



