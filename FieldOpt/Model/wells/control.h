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

#ifndef CONTROL_H
#define CONTROL_H

#include "Settings/settings.h"
#include "Settings/model.h"
#include "Model/properties/discrete_property.h"
#include "Model/properties/continous_property.h"
#include "Model/properties/binary_property.h"
#include "Model/properties/variable_property_container.h"

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
  Control(::Settings::Model::Well::ControlEntry entry,
          ::Settings::Model::Well well,
          ::Model::Properties::VariablePropertyContainer *variables);

  /*!
   * Copy constructor.
   * @param other control to be copied.
   */
  Control(const Control & other);

  /*!
   * Empty default constructor.
   */
  Control();


  int time_step() const { return time_step_->value(); }

  bool open() const { return open_->value(); }
  void setOpen(bool open) { open_->setValue(open); }

  double bhp() const { return bhp_->value(); }
  void setBhp(double bhp) { bhp_->setValue(bhp); }

  double rate()          const { return liq_rate_->value(); }
  double liquidRate()    const { return liq_rate_->value(); }
  double oilRate()       const { return oil_rate_->value(); }
  double gasRate()       const { return gas_rate_->value(); }
  double waterRate()     const { return wat_rate_->value(); }
  double reservoirRate() const { return res_rate_->value(); }

  void setRate(double rate)       { liq_rate_->setValue(rate); }
  void setLiquidRate(double rate) { liq_rate_->setValue(rate); }

  ::Settings::Model::ControlMode mode() const { return mode_; }
  ::Settings::Model::InjectionType injection_fluid() const { return injection_fluid_; }

 private:
  Properties::DiscreteProperty *time_step_;
  Properties::BinaryProperty *open_;
  Properties::ContinousProperty *bhp_;
  Properties::ContinousProperty *liq_rate_;
  Properties::ContinousProperty *oil_rate_;
  Properties::ContinousProperty *gas_rate_;
  Properties::ContinousProperty *wat_rate_;
  Properties::ContinousProperty *res_rate_;
  ::Settings::Model::ControlMode mode_;
  ::Settings::Model::InjectionType injection_fluid_;
};
}
}
#endif // CONTROL_H
