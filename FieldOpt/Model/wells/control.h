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

#ifndef CONTROL_H
#define CONTROL_H

#include "Utilities/settings/settings.h"
#include "Utilities/settings/model.h"
#include "Model/properties/discrete_property.h"
#include "Model/properties/continous_property.h"
#include "Model/properties/binary_property.h"
#include "Model/properties/variable_property_container.h"
#include "Model/properties/variable_property_handler.h"

namespace Model {
namespace Wells {

/*!
 * \brief The Control class represents a well control.
 */
class Control
{
public:
    /*!
     * \brief Control
     * \param entry The settings entry to create a control from.
     * \param well The well to which the settings/control belongs.
     * \param variables The variable container to which any created variables that are declared _variable_ should be added.
     * \param variable_handler Used to check if a control has been declared _variable_.
     */
    Control(::Utilities::Settings::Model::ControlEntry entry,
            ::Utilities::Settings::Model::Well well,
            ::Model::Properties::VariablePropertyContainer *variables,
            ::Model::Properties::VariablePropertyHandler *variable_handler);

    int time_step() const { return time_step_->value(); }

    bool open() const { return open_->value(); }
    void setOpen(bool open) { open_->setValue(open); }

    double bhp() const { return bhp_->value(); }
    void setBhp(double bhp) { bhp_->setValue(bhp); }

    double rate() const { return rate_->value(); }
    void setRate(double rate) { rate_->setValue(rate); }

    ::Utilities::Settings::Model::ControlMode mode() const { return mode_; }
    ::Utilities::Settings::Model::InjectionType injection_fluid() const { return injection_fluid_; }

private:
    Properties::DiscreteProperty *time_step_;
    Properties::BinaryProperty *open_;
    Properties::ContinousProperty *bhp_;
    Properties::ContinousProperty *rate_;
    ::Utilities::Settings::Model::ControlMode mode_;
    ::Utilities::Settings::Model::InjectionType injection_fluid_;
};
}
}
#endif // CONTROL_H
