/******************************************************************************
   Created by Brage on 31/03/19.
   Copyright (C) 2018 Brage Strand Kristoffersen <brage_sk@hotmail.com>

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
#include "reservoir_xyz_boundary.h"
#include "ConstraintMath/well_constraint_projections/well_constraint_projections.h"
#include <iomanip>
#include "Utilities/math.hpp"

namespace Optimization {
namespace Constraints {
ReservoirXYZBoundary::ReservoirXYZBoundary(const Settings::Optimizer::Constraint &settings,
                                           Model::Properties::VariablePropertyContainer *variables,
                                           Reservoir::Grid::Grid *grid) {

  xmin_ = settings.box_xyz_xmin;
  xmax_ = settings.box_xyz_xmax;
  ymin_ = settings.box_xyz_ymin;
  ymax_ = settings.box_xyz_ymax;
  zmin_ = settings.box_xyz_zmin;
  zmax_ = settings.box_xyz_zmax;
  grid_ = grid;
  penalty_weight_ = settings.penalty_weight;

  if (!variables->GetWellSplineVariables(settings.well).empty())
    affected_well_ = initializeWell(variables->GetWellSplineVariables(settings.well));
  else
    affected_well_ = initializeWell(variables->GetWellSplineVariables(settings.well));
}

bool ReservoirXYZBoundary::CaseSatisfiesConstraint(Case *c) {

  double heel_x_val = c->real_variables()[affected_well_.heel.x];
  double heel_y_val = c->real_variables()[affected_well_.heel.y];
  double heel_z_val = c->real_variables()[affected_well_.heel.z];

  double toe_x_val = c->real_variables()[affected_well_.toe.x];
  double toe_y_val = c->real_variables()[affected_well_.toe.y];
  double toe_z_val = c->real_variables()[affected_well_.toe.z];

  bool heel_feasible = false;
  bool toe_feasible = false;

  for (int ii = 0; ii < index_list_.length(); ii++) {
    if (grid_->GetCell(index_list_[ii]).EnvelopsPoint(
        Eigen::Vector3d(heel_x_val, heel_y_val, heel_z_val))) {
      heel_feasible = true;
    }
    if (grid_->GetCell(index_list_[ii]).EnvelopsPoint(
        Eigen::Vector3d(toe_x_val, toe_y_val, toe_z_val))) {
      toe_feasible = true;
    }
  }

  return heel_feasible && toe_feasible;
}

void ReservoirXYZBoundary::SnapCaseToConstraints(Case *c) {

  double heel_x_val = c->real_variables()[affected_well_.heel.x];
  double heel_y_val = c->real_variables()[affected_well_.heel.y];
  double heel_z_val = c->real_variables()[affected_well_.heel.z];

  double toe_x_val = c->real_variables()[affected_well_.toe.x];
  double toe_y_val = c->real_variables()[affected_well_.toe.y];
  double toe_z_val = c->real_variables()[affected_well_.toe.z];

  Eigen::Vector3d projected_heel =
      WellConstraintProjections::well_domain_constraint_indices(
          Eigen::Vector3d(heel_x_val, heel_y_val, heel_z_val),
          grid_,
          index_list_
      );
  Eigen::Vector3d projected_toe =
      WellConstraintProjections::well_domain_constraint_indices(
          Eigen::Vector3d(toe_x_val, toe_y_val, toe_z_val),
          grid_,
          index_list_
      );

  c->set_real_variable_value(affected_well_.heel.x, projected_heel(0));
  c->set_real_variable_value(affected_well_.heel.y, projected_heel(1));
  c->set_real_variable_value(affected_well_.heel.z, projected_heel(2));

  c->set_real_variable_value(affected_well_.toe.x, projected_toe(0));
  c->set_real_variable_value(affected_well_.toe.y, projected_toe(1));
  c->set_real_variable_value(affected_well_.toe.z, projected_toe(2));
}

Eigen::VectorXd ReservoirXYZBoundary::GetLowerBounds(QList<QUuid> id_vector) const {
  Eigen::VectorXd lbounds(id_vector.size());
  lbounds.fill(0);

  int ind_heel_x = id_vector.indexOf(affected_well_.heel.x);
  int ind_heel_y = id_vector.indexOf(affected_well_.heel.y);
  int ind_heel_z = id_vector.indexOf(affected_well_.heel.z);
  int ind_toe_x = id_vector.indexOf(affected_well_.toe.x);
  int ind_toe_y = id_vector.indexOf(affected_well_.toe.y);
  int ind_toe_z = id_vector.indexOf(affected_well_.toe.z);
  lbounds(ind_heel_x) = xmin_;
  lbounds(ind_toe_x) = xmin_;
  lbounds(ind_heel_y) = ymin_;
  lbounds(ind_toe_y) = ymin_;
  lbounds(ind_heel_z) = zmin_;
  lbounds(ind_toe_z) = zmin_;

  return lbounds;
}

Eigen::VectorXd ReservoirXYZBoundary::GetUpperBounds(QList<QUuid> id_vector) const {
  Eigen::VectorXd ubounds(id_vector.size());
  ubounds.fill(0);

  int ind_heel_x = id_vector.indexOf(affected_well_.heel.x);
  int ind_heel_y = id_vector.indexOf(affected_well_.heel.y);
  int ind_heel_z = id_vector.indexOf(affected_well_.heel.z);
  int ind_toe_x = id_vector.indexOf(affected_well_.toe.x);
  int ind_toe_y = id_vector.indexOf(affected_well_.toe.y);
  int ind_toe_z = id_vector.indexOf(affected_well_.toe.z);
  ubounds(ind_heel_x) = xmax_;
  ubounds(ind_toe_x) = xmax_;
  ubounds(ind_heel_y) = ymax_;
  ubounds(ind_toe_y) = ymax_;
  ubounds(ind_heel_z) = zmax_;
  ubounds(ind_toe_z) = zmax_;
  return ubounds;
}

}
}
