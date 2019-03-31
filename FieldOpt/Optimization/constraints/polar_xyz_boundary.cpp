/******************************************************************************
   Created by Brage on 31/03/19.
   Copyright (C) 2019 Brage Strand Kristoffersen <brage_sk@hotmail.com>

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


#include "polar_xyz_boundary.h"
#include "ConstraintMath/well_constraint_projections/well_constraint_projections.h"
#include <iomanip>
#include "Utilities/math.hpp"

namespace Optimization {
namespace Constraints {
PolarXYZBoundary::PolarXYZBoundary(const Settings::Optimizer::Constraint &settings,
                                       Model::Properties::VariablePropertyContainer *variables,
                                       Reservoir::Grid::Grid *grid)
{
    xmin_ = settings.box_xyz_xmin;
    xmax_ = settings.box_xyz_xmax;
    ymin_ = settings.box_xyz_ymin;
    ymax_ = settings.box_xyz_ymax;
    zmin_ = settings.box_xyz_zmin;
    zmax_ = settings.box_xyz_zmax;
    grid_ = grid;
    penalty_weight_ = settings.penalty_weight;

    if (!variables->GetPolarSplineVariables(settings.well).empty())
        affected_well_ = initializeWell(variables->GetPolarSplineVariables(settings.well));
    else
        affected_well_ = initializeWell(variables->GetPolarSplineVariables(settings.well));


}

bool PolarXYZBoundary::CaseSatisfiesConstraint(Case *c) {

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

void PolarXYZBoundary::SnapCaseToConstraints(Case *c) {
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

Eigen::VectorXd PolarXYZBoundary::GetLowerBounds(QList<QUuid> id_vector) const {

    Eigen::VectorXd lbounds(id_vector.size());
    lbounds.fill(0);

    int ind_midpoint_x = id_vector.indexOf(affected_well_.midpoint.x);
    int ind_midpoint_y = id_vector.indexOf(affected_well_.midpoint.y);
    int ind_midpoint_z = id_vector.indexOf(affected_well_.midpoint.z);
    lbounds(ind_midpoint_x) = xmin_;
    lbounds(ind_midpoint_y) = ymin_;
    lbounds(ind_midpoint_z) = zmin_;
    return lbounds;
}

Eigen::VectorXd PolarXYZBoundary::GetUpperBounds(QList<QUuid> id_vector) const {
    Eigen::VectorXd ubounds(id_vector.size());
    ubounds.fill(0);

    int ind_midpoint_x = id_vector.indexOf(affected_well_.midpoint.x);
    int ind_midpoint_y = id_vector.indexOf(affected_well_.midpoint.y);
    int ind_midpoint_z = id_vector.indexOf(affected_well_.midpoint.z);
    ubounds(ind_midpoint_x) = xmax_;
    ubounds(ind_midpoint_y) = ymax_;
    ubounds(ind_midpoint_z) = zmax_;
    return ubounds;
}

}
}

