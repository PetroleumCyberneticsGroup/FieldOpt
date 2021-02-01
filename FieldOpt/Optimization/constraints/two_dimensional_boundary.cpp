//
// Created by brage on 21.01.2021.
//

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
#include "two_dimensional_boundary.h"
#include "ConstraintMath/well_constraint_projections/well_constraint_projections.h"
#include <iomanip>
#include "Utilities/math.hpp"

namespace Optimization {
namespace Constraints {
TwoDimensionalBoundary::TwoDimensionalBoundary(const Settings::Optimizer::Constraint &settings,
                                           Model::Properties::VariablePropertyContainer *variables,
                                           Reservoir::Grid::Grid *grid) {

    xmin_ = settings.box_xyz_xmin;
    xmax_ = settings.box_xyz_xmax;
    ymin_ = settings.box_xyz_ymin;
    ymax_ = settings.box_xyz_ymax;
    grid_ = grid;
    penalty_weight_ = settings.penalty_weight;
    settings_ = settings;

    if (!variables->GetWellSplineVariables(settings.well).empty())
        affected_well_ = initializeWell(variables->GetAWPSplineVariables(settings.well));
    else
        affected_well_ = initializeWell(variables->GetAWPSplineVariables(settings.well));
}

bool TwoDimensionalBoundary::CaseSatisfiesConstraint(Case *c) {

    double heel_x_val = c->real_variables()[affected_well_.heel.x];
    double heel_y_val = c->real_variables()[affected_well_.heel.y];
    double toe_x_val = c->real_variables()[affected_well_.toe.x];
    double toe_y_val = c->real_variables()[affected_well_.toe.y];

    bool heel_feasible = true;
    bool toe_feasible = true;
    if (settings_.box_xyz_xmax < heel_x_val){
        heel_feasible = false;
    }
    if (settings_.box_xyz_xmin > heel_x_val){
        heel_feasible = false;
    }
    if (settings_.box_xyz_ymax < heel_y_val){
        heel_feasible = false;
    }
    if (settings_.box_xyz_ymin > heel_y_val){
        heel_feasible = false;
    }
    if (settings_.box_xyz_xmax < toe_x_val){
        toe_feasible = false;
    }
    if (settings_.box_xyz_xmin > toe_x_val){
        toe_feasible = false;
    }
    if (settings_.box_xyz_ymax < toe_y_val){
        toe_feasible = false;
    }
    if (settings_.box_xyz_ymin > toe_y_val) {
        toe_feasible = false;
    }
    return heel_feasible && toe_feasible;
}

void TwoDimensionalBoundary::SnapCaseToConstraints(Case *c) {
    double heel_x_val = c->real_variables()[affected_well_.heel.x];
    double heel_y_val = c->real_variables()[affected_well_.heel.y];

    double toe_x_val = c->real_variables()[affected_well_.toe.x];
    double toe_y_val = c->real_variables()[affected_well_.toe.y];

    if (settings_.box_xyz_xmax < heel_x_val){
        heel_x_val = settings_.box_xyz_xmax;
    }
    if (settings_.box_xyz_xmin > heel_x_val){
        heel_x_val = settings_.box_xyz_xmin;
    }
    if (settings_.box_xyz_ymax < heel_y_val){
        heel_y_val = settings_.box_xyz_ymax;
    }
    if (settings_.box_xyz_ymin > heel_y_val){
        heel_y_val = settings_.box_xyz_ymin;
    }
    if (settings_.box_xyz_xmax < toe_x_val){
        toe_x_val = settings_.box_xyz_xmax;
    }
    if (settings_.box_xyz_xmin > toe_x_val){
        toe_x_val = settings_.box_xyz_xmin;
    }
    if (settings_.box_xyz_ymax < toe_y_val){
        toe_y_val = settings_.box_xyz_ymax;
    }
    if (settings_.box_xyz_ymin > toe_y_val){
        toe_y_val = settings_.box_xyz_ymin;
    }

    Eigen::Vector2d projected_heel = Eigen::Vector2d(heel_x_val, heel_y_val);
    Eigen::Vector2d projected_toe = Eigen::Vector2d(toe_x_val, toe_y_val);

    c->set_real_variable_value(affected_well_.heel.x, projected_heel(0));
    c->set_real_variable_value(affected_well_.heel.y, projected_heel(1));

    c->set_real_variable_value(affected_well_.toe.x, projected_toe(0));
    c->set_real_variable_value(affected_well_.toe.y, projected_toe(1));
}

Eigen::VectorXd TwoDimensionalBoundary::GetLowerBounds(QList<QUuid> id_vector) const {
    Eigen::VectorXd lbounds(id_vector.size());
    lbounds.fill(0);

    int ind_heel_x = id_vector.indexOf(affected_well_.heel.x);
    int ind_heel_y = id_vector.indexOf(affected_well_.heel.y);
    int ind_toe_x = id_vector.indexOf(affected_well_.toe.x);
    int ind_toe_y = id_vector.indexOf(affected_well_.toe.y);
    lbounds(ind_heel_x) = xmin_;
    lbounds(ind_toe_x) = xmin_;
    lbounds(ind_heel_y) = ymin_;
    lbounds(ind_toe_y) = ymin_;
    return lbounds;
}

Eigen::VectorXd TwoDimensionalBoundary::GetUpperBounds(QList<QUuid> id_vector) const {
    Eigen::VectorXd ubounds(id_vector.size());
    ubounds.fill(0);

    int ind_heel_x = id_vector.indexOf(affected_well_.heel.x);
    int ind_heel_y = id_vector.indexOf(affected_well_.heel.y);
    int ind_toe_x = id_vector.indexOf(affected_well_.toe.x);
    int ind_toe_y = id_vector.indexOf(affected_well_.toe.y);
    ubounds(ind_heel_x) = xmax_;
    ubounds(ind_toe_x) = xmax_;
    ubounds(ind_heel_y) = ymax_;
    ubounds(ind_toe_y) = ymax_;
    return ubounds;
}

}
}
