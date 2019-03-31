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
#include "reservoir_boundary_toe.h"
#include "reservoir_boundary.h"
#include "ConstraintMath/well_constraint_projections/well_constraint_projections.h"
#include <iomanip>
#include "Utilities/math.hpp"
namespace Optimization {
namespace Constraints {
ReservoirBoundaryToe::ReservoirBoundaryToe(const Settings::Optimizer::Constraint &settings,
                                         Model::Properties::VariablePropertyContainer *variables,
                                         Reservoir::Grid::Grid *grid)
                                        : ReservoirBoundary(settings, variables, grid){}
bool ReservoirBoundaryToe::CaseSatisfiesConstraint(Case *c) {
    double toe_x_val = c->real_variables()[affected_well_.toe.x];
    double toe_y_val = c->real_variables()[affected_well_.toe.y];
    double toe_z_val = c->real_variables()[affected_well_.toe.z];

    bool midpoint_feasible = false;

    for (int ii=0; ii<index_list_.length(); ii++){
        if (grid_->GetCell(index_list_[ii]).EnvelopsPoint(
                Eigen::Vector3d(toe_x_val, toe_y_val, toe_z_val))) {
            midpoint_feasible = true;
        }
    }

    return midpoint_feasible;
}
void ReservoirBoundaryToe::SnapCaseToConstraints(Case *c) {

    double toe_x_val = c->real_variables()[affected_well_.toe.x];
    double toe_y_val = c->real_variables()[affected_well_.toe.y];
    double toe_z_val = c->real_variables()[affected_well_.toe.z];

    Eigen::Vector3d projected_toe =
            WellConstraintProjections::well_domain_constraint_indices(
                    Eigen::Vector3d(toe_x_val, toe_y_val, toe_z_val),
                    grid_,
                    index_list_
            );

    c->set_real_variable_value(affected_well_.toe.x, projected_toe(0));
    c->set_real_variable_value(affected_well_.toe.y, projected_toe(1));
    c->set_real_variable_value(affected_well_.toe.z, projected_toe(2));

}
Eigen::VectorXd ReservoirBoundaryToe::GetLowerBounds(QList<QUuid> id_vector) const {
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

    int ind_toe_x = id_vector.indexOf(affected_well_.toe.x);
    int ind_toe_y = id_vector.indexOf(affected_well_.toe.y);
    int ind_toe_z = id_vector.indexOf(affected_well_.toe.z);
    lbounds(ind_toe_x) = xmin;
    lbounds(ind_toe_y) = ymin;
    lbounds(ind_toe_z) = zmin;
    return lbounds;
}

Eigen::VectorXd ReservoirBoundaryToe::GetUpperBounds(QList<QUuid> id_vector) const {
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

    int ind_toe_x = id_vector.indexOf(affected_well_.toe.x);
    int ind_toe_y = id_vector.indexOf(affected_well_.toe.y);
    int ind_toe_z = id_vector.indexOf(affected_well_.toe.z);
    ubounds(ind_toe_x) = xmax;
    ubounds(ind_toe_y) = ymax;
    ubounds(ind_toe_z) = zmax;
    return ubounds;
}
}
}

