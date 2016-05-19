/******************************************************************************
 *
 * well.cpp
 *
 * Created: 22.09.2015 2015 by einar
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

#include "well.h"

namespace Model {
namespace Wells {

Well::Well(Utilities::Settings::Model settings,
           int well_number,
           Properties::VariablePropertyContainer *variable_container,
           ::Model::Properties::VariablePropertyHandler *variable_handler,
           Reservoir::Reservoir *reservoir)
{
    Utilities::Settings::Model::Well well_settings = settings.wells().at(well_number);

    name_ = well_settings.name;
    type_ = well_settings.type;
    prefered_phase_ = well_settings.prefered_phase;

    heel_.i = new Properties::DiscreteProperty(well_settings.heel.i);
    heel_.j = new Properties::DiscreteProperty(well_settings.heel.j);
    heel_.k = new Properties::DiscreteProperty(well_settings.heel.k);

    wellbore_radius_ = new Properties::ContinousProperty(well_settings.wellbore_radius);

    controls_ = new QList<Control *>();
    for (int i = 0; i < well_settings.controls.size(); ++i)
        controls_->append(new Control(well_settings.controls[i], well_settings, variable_container, variable_handler));

    trajectory_ = new Wellbore::Trajectory(well_settings, variable_container, variable_handler, reservoir);
}

bool Well::IsProducer()
{
    if (type_ == ::Utilities::Settings::Model::WellType::Producer)
        return true;
    else
        return false;
}

bool Well::IsInjector()
{
    if (type_ == ::Utilities::Settings::Model::WellType::Injector)
        return true;
    else
        return false;
}

}
}
