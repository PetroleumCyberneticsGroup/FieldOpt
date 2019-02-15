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

#include "polar_spline.h"
#include <math.h>


namespace Model {
namespace Wells {
namespace Wellbore {


PolarSpline::PolarSpline(::Settings::Model::Well well_settings,
         Properties::VariablePropertyContainer *variable_container,
         Reservoir::Grid::Grid *grid,
         Reservoir::WellIndexCalculation::wicalc_rixx *wic )
{
    // TODO: Initialize all the variables
    // TODO: Initialize spline_points_ list with three members.

    grid_ = grid;
    assert(grid_ != nullptr);
    well_settings_ = well_settings;
    is_variable_ = false;
    use_bezier_spline_ = well_settings.use_bezier_spline;
    if (wic == nullptr) { // Initialize WIC if this is the first spline well initialized.
        wic = new Reservoir::WellIndexCalculation::wicalc_rixx(grid_);
        wic_ = wic;
    }
    else { // If not, use existing WIC object.
        wic_ = wic;
    }

    bool variable = well_settings.polar_spline.is_variable;
    midpoint_ = new SplinePoint();
    midpoint_->x = new Properties::ContinousProperty(well_settings.polar_spline.midpoint.x);
    midpoint_->y = new Properties::ContinousProperty(well_settings.polar_spline.midpoint.y);
    midpoint_->z = new Properties::ContinousProperty(well_settings.polar_spline.midpoint.z);

    length_ = new Properties::ContinousProperty(well_settings.polar_spline.length);
    azimuth_ = new Properties::ContinousProperty(well_settings.polar_spline.azimuth*M_PI/180.0);
    elevation_ = new Properties::ContinousProperty(well_settings.polar_spline.elevation*M_PI/180.0);

    QString base_name = "PolarSpline#" + well_settings.name + "#";
    if (variable) {
        midpoint_->x->setName(base_name + "Midpoint#x");
        midpoint_->y->setName(base_name + "Midpoint#y");
        midpoint_->z->setName(base_name + "Midpoint#z");
        azimuth_->setName(base_name + "Azimuth");
        length_->setName(base_name + "Length");
        elevation_->setName(base_name + "Elevation");
        variable_container->AddVariable(midpoint_->x);
        variable_container->AddVariable(midpoint_->y);
        variable_container->AddVariable(midpoint_->z);
        variable_container->AddVariable(azimuth_);
        variable_container->AddVariable(length_);
        variable_container->AddVariable(elevation_);
    }
    spline_points_ = QList<SplinePoint*>();
    computePoints();
}

QList<WellBlock *> *PolarSpline::GetWellBlocks() {
    computePoints();
    return computeWellBlocks();
}


void PolarSpline::computePoints() {
    double r = length_->value() / 2.0;
    auto p0 = midpoint_->ToEigenVector();
    Eigen::Vector3d p1, p2;

    p1[0] = r * sin(elevation_->value()) * cos(azimuth_->value());
    p1[1] = r * sin(elevation_->value()) * sin(azimuth_->value());
    p1[2] = r * cos(elevation_->value());

    p2[0] = -1.0 * r * sin(elevation_->value()) * cos(azimuth_->value());
    p2[1] = -1.0 * r * sin(elevation_->value()) * sin(azimuth_->value());
    p2[2] = -1.0 * r * cos(elevation_->value());

    p1 = p1 + midpoint_->ToEigenVector();
    p2 = p2 + midpoint_->ToEigenVector();

    if (spline_points_.size() == 0) {
        SplinePoint *sp1 = new SplinePoint();
        sp1->x = new Properties::ContinousProperty(p1.x());
        sp1->y = new Properties::ContinousProperty(p1.y());
        sp1->z = new Properties::ContinousProperty(p1.z());
        SplinePoint *sp2 = new SplinePoint();
        sp2->x = new Properties::ContinousProperty(p2.x());
        sp2->y = new Properties::ContinousProperty(p2.y());
        sp2->z = new Properties::ContinousProperty(p2.z());

        spline_points_.append(sp1);
        spline_points_.append(midpoint_);
        spline_points_.append(sp2);
    }
    else {
        spline_points_[0]->FromEigenVector(p1);
        spline_points_[1] = midpoint_;
        spline_points_[2]->FromEigenVector(p2);
    }
}

}
}
}
