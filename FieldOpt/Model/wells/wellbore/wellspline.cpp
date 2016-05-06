/******************************************************************************
 *
 * wellspline.cpp
 *
 * Created: 24.09.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#include "wellspline.h"
#include <iostream>

namespace Model {
namespace Wells {
namespace Wellbore {

WellSpline::WellSpline(Utilities::Settings::Model::Well well_settings, Properties::VariablePropertyContainer *variable_container, Properties::VariablePropertyHandler *variable_handler, Reservoir::Reservoir *reservoir)
{
    block_id_ = 0;
    grid_ = reservoir->grid();
    well_settings_ = well_settings;

    if (well_settings.spline_points.length() > 2)
        throw std::runtime_error("Wells defined by a spline with more than two points are not yet supported!");

    heel_x_ = new Model::Properties::ContinousProperty(well_settings.spline_points.first().x);
    heel_y_ = new Model::Properties::ContinousProperty(well_settings.spline_points.first().y);
    heel_z_ = new Model::Properties::ContinousProperty(well_settings.spline_points.first().z);
    toe_x_ = new Model::Properties::ContinousProperty(well_settings.spline_points.last().x);
    toe_y_ = new Model::Properties::ContinousProperty(well_settings.spline_points.last().y);
    toe_z_ = new Model::Properties::ContinousProperty(well_settings.spline_points.last().z);

    if (variable_handler->GetSplinePoint(well_settings.name, 0)) { // Check if heel is variable
        QString base_name = variable_handler->GetSplinePoint(well_settings.name, 0)->variable_name() + "#" + well_settings.name + "#heel";
        heel_x_->setName(base_name + "#x");
        heel_y_->setName(base_name + "#y");
        heel_z_->setName(base_name + "#z");
        variable_container->AddVariable(heel_x_);
        variable_container->AddVariable(heel_y_);
        variable_container->AddVariable(heel_z_);
    }
    if (variable_handler->GetSplinePoint(well_settings.name, 1)) { // Check if toe is variable
        QString base_name = variable_handler->GetSplinePoint(well_settings.name, 1)->variable_name() + "#" + well_settings.name + "#toe";
        toe_x_->setName(base_name + "#x");
        toe_y_->setName(base_name + "#y");
        toe_z_->setName(base_name + "#z");
        variable_container->AddVariable(toe_x_);
        variable_container->AddVariable(toe_y_);
        variable_container->AddVariable(toe_z_);
    }
}

QList<WellBlock *> *WellSpline::GetWellBlocks()
{
    Model::Reservoir::Grid::XYZCoordinate heel = Model::Reservoir::Grid::XYZCoordinate(heel_x_->value(), heel_y_->value(), heel_z_->value());
    Model::Reservoir::Grid::XYZCoordinate toe = Model::Reservoir::Grid::XYZCoordinate(toe_x_->value(), toe_y_->value(), toe_z_->value());

    ::WellIndexCalculator::WellIndexCalculator wic = ::WellIndexCalculator::WellIndexCalculator(grid_, well_settings_.wellbore_radius);
    QList<Model::Reservoir::Grid::XYZCoordinate> points = QList<Model::Reservoir::Grid::XYZCoordinate>();
    points.append(heel);
    points.append(toe);
    QList<WellIndexCalculator::WellIndexCalculator::BlockData> block_data = wic.GetBlocks(points);
    QList<WellBlock *> *blocks = new QList<WellBlock *>();
    for (int i = 0; i < block_data.length(); ++i) {
        blocks->append(getWellBlock(block_data[i]));
    }
    return blocks;
}

WellBlock *WellSpline::getWellBlock(WellIndexCalculator::WellIndexCalculator::BlockData block_data)
{
    WellBlock *wb = new WellBlock(block_data.i+1, block_data.j+1, block_data.k+1, block_id_++);
    auto comp = new Completions::Perforation();
    comp->setTransmissibility_factor(block_data.well_index);
    wb->AddCompletion(comp);
    return wb;
}

}
}
}
