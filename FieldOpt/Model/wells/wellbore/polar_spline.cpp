#include "polar_spline.h"
#include <math.h>


namespace Model {
namespace Wells {
namespace Wellbore {


PolarSpline::PolarSpline(::Settings::Model::Well well_settings,
         Properties::VariablePropertyContainer *variable_container,
         Reservoir::Grid::Grid *grid,
         Reservoir::WellIndexCalculation::wicalc_rixx *wic )
: WellSpline(well_settings, variable_container, grid, wic)
{
    // TODO: Initialize all the variables
    // TODO: Initialize spline_points_ list with three members.
    midpoint_->x->setValue(1);
    midpoint_->y->setValue(1);
    midpoint_->z->setValue(1);
    azimuth_->setValue(90);
    length_->setValue(100);
    elevation_->setValue(45);
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

    p2[0] = -1.0*r * sin(elevation_->value()) * cos(azimuth_->value());
    p2[1] = -1.0*r * sin(elevation_->value()) * sin(azimuth_->value());
    p2[2] = -1.0*r * cos(elevation_->value());

    spline_points_[0]->FromEigenVector(p1);
    spline_points_[1] = midpoint_;
    spline_points_[2]->FromEigenVector(p2);
}

}
}
}
