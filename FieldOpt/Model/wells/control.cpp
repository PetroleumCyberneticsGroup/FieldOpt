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
                 ::Model::Variables::VariableContainer *variables,
                 ::Model::Variables::VariableHandler *variable_handler)
{
    time_step_ = new Variables::IntegerVariable(entry.time_step);

    if (well.type == ::Utilities::Settings::Model::WellType::Injector)
        injection_fluid_ = entry.injection_type;

    // Open/Closed
    if (entry.state == ::Utilities::Settings::Model::WellState::WellOpen)
        open_ = new Variables::BinaryVariable(true);
    else if (entry.state == ::Utilities::Settings::Model::WellState::WellShut)
        open_ = new Variables::BinaryVariable(false);
    if (variable_handler->GetControl(well.name, entry.time_step)->open())
        variables->AddVariable(open_);

    switch (entry.control_mode) {
    case ::Utilities::Settings::Model::ControlMode::BHPControl:
        mode_ = entry.control_mode;
        bhp_ = new Variables::RealVariable(entry.bhp);
        if (variable_handler->GetControl(well.name, entry.time_step)->bhp())
            variables->AddVariable(bhp_);
        break;
    case ::Utilities::Settings::Model::ControlMode::RateControl:
        mode_ = entry.control_mode;
        rate_ = new Variables::RealVariable(entry.rate);
        if (variable_handler->GetControl(well.name, entry.time_step)->rate())
            variables->AddVariable(rate_);
    }


}

int Control::time_step() const
{
    return time_step_->value();
}

bool Control::open() const
{
    return open_->value();
}

void Control::setOpen(bool open)
{
    open_->setValue(open);
}

double Control::bhp() const
{
    return bhp_->value();
}

void Control::setBhp(double bhp)
{
    bhp_->setValue(bhp);
}

double Control::rate() const
{
    return rate_->value();
}

void Control::setRate(double rate)
{
    rate_->setValue(rate);
}

::Utilities::Settings::Model::ControlMode Control::mode() const
{
    return mode_;
}

::Utilities::Settings::Model::InjectionType Control::injection_fluid() const
{
    return injection_fluid_;
}


}
}
