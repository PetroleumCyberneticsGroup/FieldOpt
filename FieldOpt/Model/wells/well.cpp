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

#include "well.h"

namespace Model {
namespace Wells {

Well::Well(Settings::Model settings,
           int well_number,
           Properties::VariablePropertyContainer *variable_container,
           Reservoir::Grid::Grid *grid)
{
    Settings::Model::Well well_settings = settings.wells().at(well_number);

    name_ = well_settings.name;
    type_ = well_settings.type;
    if (well_settings.group.length() >= 1) {
        group_ = well_settings.group;
    } else {
        group_ = "";
    }

    preferred_phase_ = well_settings.preferred_phase;
    wellbore_radius_ = new Properties::ContinousProperty(
        well_settings.wellbore_radius);

    controls_ = new QList<Control *>();
    for (int i = 0; i < well_settings.controls.size(); ++i) {
        controls_->append(new Control(well_settings.controls[i],
                                      well_settings, variable_container));
    }

    trajectory_ = new Wellbore::Trajectory(well_settings,
                                           variable_container,
                                           grid);

    heel_.i = trajectory_->GetWellBlocks()->first()->i();
    heel_.j = trajectory_->GetWellBlocks()->first()->j();
    heel_.k = trajectory_->GetWellBlocks()->first()->k();
}

bool Well::IsProducer()
{
    return type_ == ::Settings::Model::WellType::Producer;
}

bool Well::IsInjector()
{
    return type_ == ::Settings::Model::WellType::Injector;
}

void Well::Update() {
    trajectory_->UpdateWellBlocks();
    heel_.i = trajectory_->GetWellBlocks()->first()->i();
    heel_.j = trajectory_->GetWellBlocks()->first()->j();
    heel_.k = trajectory_->GetWellBlocks()->first()->k();
}

void Well::Update(int rank) {
    trajectory_->UpdateWellBlocks(rank);
    heel_.i = trajectory_->GetWellBlocks()->first()->i();
    heel_.j = trajectory_->GetWellBlocks()->first()->j();
    heel_.k = trajectory_->GetWellBlocks()->first()->k();
}


}
}
