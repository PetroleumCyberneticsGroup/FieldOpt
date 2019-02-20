/******************************************************************************
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>,
   Brage Strand Kristoffersen <brage_sk@hotmail.com>

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

#include "polar_spline_boundary.h"

namespace Optimization {
namespace Constraints {
PolarSplineBoundary::PolarSplineBoundary(const Settings::Optimizer::Constraint &settings,
                                         Model::Properties::VariablePropertyContainer *variables,
                                         Reservoir::Grid::Grid *grid)
                                         : ReservoirBoundary(settings, variables, grid){}
bool PolarSplineBoundary::CaseSatisfiesConstraint(Case *c) {
  double midpoint_x_val = c->real_variables()[affected_well_.midpoint.x];
  double midpoint_y_val = c->real_variables()[affected_well_.midpoint.y];
  double midpoint_z_val = c->real_variables()[affected_well_.midpoint.z];
  
  bool midpoint_feasible = false;

  for (int ii=0; ii<index_list_.length(); ii++){
    if (grid_->GetCell(index_list_[ii]).EnvelopsPoint(
        Eigen::Vector3d(midpoint_x_val, midpoint_y_val, midpoint_z_val))) {
      midpoint_feasible = true;
    }
  }

  return midpoint_feasible;
}
void PolarSplineBoundary::SnapCaseToConstraints(Case *c) {

  double midpoint_x_val = c->real_variables()[affected_well_.midpoint.x];
  double midpoint_y_val = c->real_variables()[affected_well_.midpoint.y];
  double midpoint_z_val = c->real_variables()[affected_well_.midpoint.z];
  
  Eigen::Vector3d projected_midpoint =
      WellConstraintProjections::well_domain_constraint_indices(
          Eigen::Vector3d(midpoint_x_val, midpoint_y_val, midpoint_z_val),
          grid_,
          index_list_
      );

  c->set_real_variable_value(affected_well_.midpoint.x, projected_midpoint(0));
  c->set_real_variable_value(affected_well_.midpoint.y, projected_midpoint(1));
  c->set_real_variable_value(affected_well_.midpoint.z, projected_midpoint(2));
  
}
Eigen::VectorXd PolarSplineBoundary::GetLowerBounds(QList<QUuid> id_vector) const {
  auto cell_min = grid_->GetCell(imin_, jmin_, kmin_);
  auto cell_max = grid_->GetCell(imax_, jmax_, kmax_);
  double xmin, ymin, zmin;
  xmin = std::min(cell_max.center().x(), cell_min.center().x());
  ymin = std::min(cell_max.center().y(), cell_min.center().y());
  zmin = std::min(cell_max.corners()[0].z(), cell_max.corners()[7].z());
  zmin = std::min(zmin, cell_min.corners()[0].z());
  zmin = std::min(zmin, cell_min.corners()[7].z());


  Eigen::VectorXd lbounds(id_vector.size());
  lbounds.fill(0);

  int ind_midpoint_x = id_vector.indexOf(affected_well_.midpoint.x);
  int ind_midpoint_y = id_vector.indexOf(affected_well_.midpoint.y);
  int ind_midpoint_z = id_vector.indexOf(affected_well_.midpoint.z);
  lbounds(ind_midpoint_x) = xmin;
  lbounds(ind_midpoint_y) = ymin;
  lbounds(ind_midpoint_z) = zmin;
  return lbounds;
}

Eigen::VectorXd PolarSplineBoundary::GetUpperBounds(QList<QUuid> id_vector) const {
  auto cell_min = grid_->GetCell(imin_, jmin_, kmin_);
  auto cell_max = grid_->GetCell(imax_, jmax_, kmax_);
  double xmax, ymax, zmax;
  xmax = std::max(cell_max.center().x(), cell_min.center().x());
  ymax = std::max(cell_max.center().y(), cell_min.center().y());
  zmax = std::max(cell_max.corners()[0].z(), cell_max.corners()[7].z());
  zmax = std::max(zmax, cell_min.corners()[0].z());
  zmax = std::max(zmax, cell_min.corners()[7].z());

  Eigen::VectorXd ubounds(id_vector.size());
  ubounds.fill(0);

  int ind_midpoint_x = id_vector.indexOf(affected_well_.midpoint.x);
  int ind_midpoint_y = id_vector.indexOf(affected_well_.midpoint.y);
  int ind_midpoint_z = id_vector.indexOf(affected_well_.midpoint.z);
  ubounds(ind_midpoint_x) = xmax;
  ubounds(ind_midpoint_y) = ymax;
  ubounds(ind_midpoint_z) = zmax;
  return ubounds;
}
}
}

