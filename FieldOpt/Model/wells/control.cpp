/******************************************************************************
 *
 *
 *
 * Created: 19.10.2015 2015 by einar
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

#include "control.h"

namespace Model {
namespace Wells {

Control::Control(::Utilities::Settings::Model::ControlEntry entry,
                 ::Utilities::Settings::Model::Well well,
                 ::Model::Properties::VariablePropertyContainer *variables,
                 ::Model::Properties::VariablePropertyHandler *variable_handler)
{
    time_step_ = new Properties::DiscreteProperty(entry.time_step);

    if (well.type == ::Utilities::Settings::Model::WellType::Injector)
        injection_fluid_ = entry.injection_type;

    // Open/Closed
    if (entry.state == ::Utilities::Settings::Model::WellState::WellOpen)
        open_ = new Properties::BinaryProperty(true);
    else if (entry.state == ::Utilities::Settings::Model::WellState::WellShut)
        open_ = new Properties::BinaryProperty(false);
    if (variable_handler->GetControl(well.name, entry.time_step)->open()) {
        open_->setName(variable_handler->GetControl(well.name, entry.time_step)->variable_name());
        variables->AddVariable(open_);
    }

    switch (entry.control_mode) {
    case ::Utilities::Settings::Model::ControlMode::BHPControl:
        mode_ = entry.control_mode;
        bhp_ = new Properties::ContinousProperty(entry.bhp);
        if (variable_handler->GetControl(well.name, entry.time_step)->bhp()) {
            QString var_name_suffix = "#" + well.name + "#" + entry.time_step;
            bhp_->setName(variable_handler->GetControl(well.name, entry.time_step)->variable_name() + var_name_suffix);
            variables->AddVariable(bhp_);
        }
        break;
    case ::Utilities::Settings::Model::ControlMode::RateControl:
        mode_ = entry.control_mode;
        rate_ = new Properties::ContinousProperty(entry.rate);
        if (variable_handler->GetControl(well.name, entry.time_step)->rate()) {
            rate_->setName(variable_handler->GetControl(well.name, entry.time_step)->variable_name());
            variables->AddVariable(rate_);
        }
    }


}

}
}
