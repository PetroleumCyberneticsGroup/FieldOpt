/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include "wellspline.h"
#include <iostream>
#include <time.h>
#include <wells/well_exceptions.h>
#include <QtCore/QDateTime>
#include <Utilities/time.hpp>
#include <boost/algorithm/string.hpp>

namespace Model {
namespace Wells {
namespace Wellbore {
using namespace Reservoir::WellIndexCalculation;

WellSpline::WellSpline(Settings::Model::Well well_settings,
                       Properties::VariablePropertyContainer *variable_container,
                       Reservoir::Grid::Grid *grid)
{
    grid_ = grid;
    well_settings_ = well_settings;
    is_variable_ = false;

    for (auto point : well_settings.spline_points) {
        SplinePoint *pt = new SplinePoint();
        pt->x = new ContinousProperty(point.x);
        pt->y = new ContinousProperty(point.y);
        pt->z = new ContinousProperty(point.z);
        pt->x->setName(point.name + "#x");
        pt->y->setName(point.name + "#y");
        pt->z->setName(point.name + "#z");
        if (point.is_variable) {
            is_variable_ = true;
            variable_container->AddVariable(pt->x);
            variable_container->AddVariable(pt->y);
            variable_container->AddVariable(pt->z);
        }
        spline_points_.push_back(pt);
    }
    seconds_spent_in_compute_wellblocks_ = 0;

    last_computed_grid_ = "";
    last_computed_spline_ = std::vector<Eigen::Vector3d>();
}

QList<WellBlock *> *WellSpline::GetWellBlocks()
{
    assert(spline_points_.size() >= 2);
    assert(grid_ != 0);

    last_computed_grid_ = grid_->GetGridFilePath();
    last_computed_spline_ = create_spline_point_vector();

    vector<WellDefinition> welldefs;
    welldefs.push_back(WellDefinition());
    welldefs[0].wellname = well_settings_.name.toStdString();

    for (int w = 0; w < spline_points_.size() - 1; ++w) {
        welldefs[0].radii.push_back(well_settings_.wellbore_radius);
        welldefs[0].skins.push_back(0.0);
        welldefs[0].skins.push_back(0.0);
        welldefs[0].heels.push_back(spline_points_[w]->ToEigenVector());
        welldefs[0].toes.push_back(spline_points_[w+1]->ToEigenVector());
    }

    auto wic = WellIndexCalculator(grid_);

    auto start = QDateTime::currentDateTime();
    auto block_data = wic.ComputeWellBlocks(welldefs)[well_settings_.name.toStdString()];
    auto end = QDateTime::currentDateTime();
    seconds_spent_in_compute_wellblocks_ = time_span_seconds(start, end);

    QList<WellBlock *> *blocks = new QList<WellBlock *>();
    for (int i = 0; i < block_data.size(); ++i) {
        if (block_data[i].cell_well_index_matrix() > 0.01) { // Ignoring well blocks with very low well index.
            blocks->append(getWellBlock(block_data[i]));
        }
    }
    if (blocks->size() == 0) {
        throw WellBlocksNotDefined("WIC could not compute.");
    }
    std::cout << "Done computing WIs after " << seconds_spent_in_compute_wellblocks_ << " seconds." << std::endl;
    return blocks;
}

WellBlock *WellSpline::getWellBlock(Reservoir::WellIndexCalculation::IntersectedCell block_data)
{
    auto wb = new WellBlock(block_data.ijk_index().i()+1, block_data.ijk_index().j()+1, block_data.ijk_index().k()+1);
    auto comp = new Completions::Perforation();
    comp->setTransmissibility_factor(block_data.cell_well_index_matrix());
    wb->AddCompletion(comp);
    return wb;
}
std::vector<Eigen::Vector3d> WellSpline::create_spline_point_vector() const {
    std::vector<Eigen::Vector3d> spline_points;
    for (auto point : spline_points_) {
        spline_points.push_back(point->ToEigenVector());
    }
    return spline_points;
}
bool WellSpline::HasGridChanged() const {
    return last_computed_grid_.size() == 0 || !boost::equals(last_computed_grid_, grid_->GetGridFilePath());
}
bool WellSpline::HasSplineChanged() const {
    if (last_computed_spline_.size() == 0) {
        return true;
    }

    std::vector<Eigen::Vector3d> new_spline_points;
    for (auto point : spline_points_) {
        new_spline_points.push_back(point->ToEigenVector());
    }
    assert(new_spline_points.size() == last_computed_spline_.size());

    double point_difference_sum = 0;
    for (int i = 0; i < last_computed_spline_.size(); ++i) {
        point_difference_sum += std::abs((last_computed_spline_[i] - new_spline_points[i]).norm());
    }
    return point_difference_sum < 1e-7;
}

Eigen::Vector3d WellSpline::SplinePoint::ToEigenVector() const {
    return Eigen::Vector3d(this->x->value(), this->y->value(), this->z->value());
}
}
}
}
