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

    heel_x_ = new Model::Properties::ContinousProperty(well_settings.spline_heel.x);
    heel_y_ = new Model::Properties::ContinousProperty(well_settings.spline_heel.y);
    heel_z_ = new Model::Properties::ContinousProperty(well_settings.spline_heel.z);
    toe_x_ = new Model::Properties::ContinousProperty(well_settings.spline_toe.x);
    toe_y_ = new Model::Properties::ContinousProperty(well_settings.spline_toe.y);
    toe_z_ = new Model::Properties::ContinousProperty(well_settings.spline_toe.z);
    seconds_spent_in_compute_wellblocks_ = 0;

    if (well_settings.spline_heel.is_variable) {
        heel_x_->setName(well_settings.spline_heel.name + "#x");
        heel_y_->setName(well_settings.spline_heel.name + "#y");
        heel_z_->setName(well_settings.spline_heel.name + "#z");
        variable_container->AddVariable(heel_x_);
        variable_container->AddVariable(heel_y_);
        variable_container->AddVariable(heel_z_);
    }
    if (well_settings.spline_toe.is_variable) {
        toe_x_->setName(well_settings.spline_toe.name + "#x");
        toe_y_->setName(well_settings.spline_toe.name + "#y");
        toe_z_->setName(well_settings.spline_toe.name + "#z");
        variable_container->AddVariable(toe_x_);
        variable_container->AddVariable(toe_y_);
        variable_container->AddVariable(toe_z_);
    }
}

QList<WellBlock *> *WellSpline::GetWellBlocks()
{
    auto heel = Eigen::Vector3d(heel_x_->value(), heel_y_->value(), heel_z_->value());
    auto toe = Eigen::Vector3d(toe_x_->value(), toe_y_->value(), toe_z_->value());

    std::cout << "  Calling WIC." << std::endl;
    auto wic = WellIndexCalculator(grid_);

    vector<WellDefinition> welldefs;
    welldefs.push_back(WellDefinition());
    welldefs[0].wellname = well_settings_.name.toStdString();
    welldefs[0].radii.push_back(well_settings_.wellbore_radius);
    welldefs[0].skins.push_back(0.0);
    welldefs[0].heels.push_back(heel);
    welldefs[0].toes.push_back(toe);
    welldefs[0].skins.push_back(0.0);

    auto start = QDateTime::currentDateTime();
    auto block_data = wic.ComputeWellBlocks(welldefs)[well_settings_.name.toStdString()];
    auto end = QDateTime::currentDateTime();
    seconds_spent_in_compute_wellblocks_ = time_span_seconds(start, end);

    QList<WellBlock *> *blocks = new QList<WellBlock *>();
    for (int i = 0; i < block_data.size(); ++i) {
        blocks->append(getWellBlock(block_data[i]));
    }
    if (blocks->size() == 0) {
        throw WellBlocksNotDefined("WIC could not compute.");
    }
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

}
}
}
