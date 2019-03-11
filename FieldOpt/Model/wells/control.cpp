/******************************************************************************
   Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include "control.h"

namespace Model {
namespace Wells {

Control::Control(::Settings::Model::Well::ControlEntry entry,
                 ::Settings::Model::Well well,
                 ::Model::Properties::VariablePropertyContainer *variables)
{
    time_step_ = new Properties::DiscreteProperty(entry.time_step);

    if (well.type == ::Settings::Model::WellType::Injector)
        injection_fluid_ = entry.injection_type;

    // Open/Closed
    if (entry.state == ::Settings::Model::WellState::WellOpen)
        open_ = new Properties::BinaryProperty(true);
    else if (entry.state == ::Settings::Model::WellState::WellShut)
        open_ = new Properties::BinaryProperty(false);


    liq_rate_ = new Properties::ContinousProperty(entry.liq_rate); // (-1 if not set in settings)
    oil_rate_ = new Properties::ContinousProperty(entry.oil_rate); // (-1 if not set in settings)
    gas_rate_ = new Properties::ContinousProperty(entry.gas_rate); // (-1 if not set in settings)
    wat_rate_ = new Properties::ContinousProperty(entry.wat_rate); // (-1 if not set in settings)
    res_rate_ = new Properties::ContinousProperty(entry.res_rate); // (-1 if not set in settings)
    bhp_ = new Properties::ContinousProperty(entry.bhp);           // (-1 if not set in settings)

    mode_ = entry.control_mode;
    switch (mode_) {
        case ::Settings::Model::ControlMode::BHPControl:
            if (entry.is_variable) {
                bhp_->setName(entry.name);
                variables->AddVariable(bhp_);
            }
            break;
        case ::Settings::Model::ControlMode::LRATControl:
            if (entry.is_variable) {
                liq_rate_->setName(entry.name);
                variables->AddVariable(liq_rate_);
            }
            break;
        case ::Settings::Model::ControlMode::ORATControl:
            if (entry.is_variable) {
                oil_rate_->setName(entry.name);
                variables->AddVariable(oil_rate_);
            }
            break;
        case ::Settings::Model::ControlMode::GRATControl:
            if (entry.is_variable) {
                gas_rate_->setName(entry.name);
                variables->AddVariable(gas_rate_);
            }
            break;
        case ::Settings::Model::ControlMode::WRATControl:
            if (entry.is_variable) {
                wat_rate_->setName(entry.name);
                variables->AddVariable(wat_rate_);
            }
            break;
        case ::Settings::Model::ControlMode::RESVControl:
            if (entry.is_variable) {
                res_rate_->setName(entry.name);
                variables->AddVariable(res_rate_);
            }
            break;
        default:
            throw std::runtime_error("Control mode not recognized in Model::Wells::Control.");
    }

}

Control::Control() {}

Control::Control(const Control & other) {
    this->time_step_ = other.time_step_;
    this->open_ = other.open_;
    this->bhp_ = other.bhp_;
    this->liq_rate_ = other.liq_rate_;
    this->oil_rate_ = other.oil_rate_;
    this->wat_rate_ = other.wat_rate_;
    this->gas_rate_ = other.gas_rate_;
    this->res_rate_ = other.res_rate_;
    this->mode_ = other.mode_;
    this->injection_fluid_ = other.injection_fluid_;
}

}
}
