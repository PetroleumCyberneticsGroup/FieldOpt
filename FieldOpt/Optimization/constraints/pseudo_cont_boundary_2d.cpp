/******************************************************************************
   Created by einar on 8/16/17.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include <iostream>
#include <cmath>
#include "pseudo_cont_boundary_2d.h"

namespace Optimization {
namespace Constraints {

PseudoContBoundary2D::PseudoContBoundary2D(const Settings::Optimizer::Constraint &settings,
                                           Model::Properties::VariablePropertyContainer *variables,
                                           Reservoir::Grid::Grid *grid) {

    assert(settings.well.size() > 0);
    assert(settings.box_imin < settings.box_imax);
    assert(settings.box_jmin < settings.box_jmax);
    assert(settings.box_imin <= 0);
    assert(settings.box_jmin <= 0);
    assert(settings.box_imax < grid->Dimensions().nx);
    assert(settings.box_jmax < grid->Dimensions().ny);

    well_name_ = settings.well;

    // determine x, y, z; min, max
    try {
        i_min_ = settings.box_imin;
        i_max_ = settings.box_imax;
        j_min_ = settings.box_jmin;
        j_max_ = settings.box_jmax;
        auto min_block = grid->GetCell(i_min_, j_min_, 0);
        auto max_block = grid->GetCell(i_max_, j_max_, 0);

        if (abs(min_block.center().z() - max_block.center().z()) > numeric_limits<float>::epsilon())
            throw runtime_error("Reservoir is non-flat.");

        x_min_ = min(min_block.center().x(), max_block.center().x());
        x_max_ = max(min_block.center().x(), max_block.center().x());
        y_min_ = min(min_block.center().y(), max_block.center().y());
        y_max_ = max(min_block.center().y(), max_block.center().y());
        assert(x_min_ != x_max_ && y_min_ != y_max_);

        auto affected_vars = variables->GetPseudoContVertVariables(well_name_);
        affected_x_var_id_ = affected_vars[0]->id();
        affected_y_var_id_ = affected_vars[1]->id();
    } catch (std::exception const &e) {
        cerr << "Error: Initialization of PseudoContBoundary2D constraint failed for well " << well_name_.toStdString() << endl;
        exit(1);
    }
}
bool PseudoContBoundary2D::CaseSatisfiesConstraint(Case *c) {
    if (c->real_variables()[affected_x_var_id_] < x_min_
        || c->real_variables()[affected_x_var_id_] > x_max_
        || c->real_variables()[affected_y_var_id_] < y_min_
        || c->real_variables()[affected_y_var_id_] > y_max_)
        return false;
    else return true;
}
void PseudoContBoundary2D::SnapCaseToConstraints(Case *c) {
    if (c->real_variables()[affected_x_var_id_] < x_min_)
        c->set_real_variable_value(affected_x_var_id_, x_min_);
    else if (c->real_variables()[affected_x_var_id_] > x_max_)
        c->set_real_variable_value(affected_x_var_id_, x_max_);
    else if (c->real_variables()[affected_y_var_id_] < y_min_)
        c->set_real_variable_value(affected_y_var_id_, y_min_);
    else if (c->real_variables()[affected_y_var_id_] > y_max_)
        c->set_real_variable_value(affected_y_var_id_, y_max_);
}
bool PseudoContBoundary2D::IsBoundConstraint() const {
    return true;
}
Eigen::VectorXd PseudoContBoundary2D::GetLowerBounds(QList<QUuid> id_vector) const {
    Eigen::VectorXd lbounds(id_vector.size());
    lbounds.fill(0);
    lbounds[id_vector.indexOf(affected_x_var_id_)] = x_min_;
    lbounds[id_vector.indexOf(affected_y_var_id_)] = y_min_;
    return lbounds;
}
Eigen::VectorXd PseudoContBoundary2D::GetUpperBounds(QList<QUuid> id_vector) const {
    Eigen::VectorXd ubounds(id_vector.size());
    ubounds.fill(0);
    ubounds[id_vector.indexOf(affected_x_var_id_)] = x_max_;
    ubounds[id_vector.indexOf(affected_y_var_id_)] = y_max_;
    return ubounds;
}
string PseudoContBoundary2D::name() {
    return "PseudoContBoundary2D";
}
void PseudoContBoundary2D::InitializeNormalizer(QList<Case *> cases) {
    normalizer_.set_max(1.0L);
    normalizer_.set_midpoint(0.0L);
    normalizer_.set_steepness(1.0L);
}
double PseudoContBoundary2D::Penalty(Case *c) {
    if (CaseSatisfiesConstraint(c))
        return 0.0;
    else
        return INFINITY;
}
long double PseudoContBoundary2D::PenaltyNormalized(Case *c) {
    if (CaseSatisfiesConstraint(c))
        return 0.0L;
    else
        return 1.0L;
}

}
}
