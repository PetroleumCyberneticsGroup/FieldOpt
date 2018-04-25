/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version
 3 of the License, or (at your option) any later version.

 FieldOpt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the
 GNU General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include "control.h"
#include <iostream>

// ---------------------------------------------------------
namespace Model {
namespace Wells {

// ---------------------------------------------------------
using std::cout;
using std::endl;

// =========================================================
Control::Control(
    ::Settings::Model::Well::ControlEntry entry,
    ::Settings::Model::Well well,
    ::Model::Properties::VariablePropertyContainer *variables) {

  // -------------------------------------------------------
  if (well.verb_vector_[5] > 4) // idx:5 -> mod
    cout << "[mod:Control.cpp]------------ "
        << "time_step: " << entry.time_step;
//         << entry.ControlEntryStr().toStdString()
//         << endl;

  // -------------------------------------------------------
  // time_step_ =
  //    new Properties::DiscreteProperty(entry.time_step);
  time_step_ =
      new Properties::ContinousProperty(entry.time_step);

  // -------------------------------------------------------
  if (well.type == ::Settings::Model::WellType::Injector) {
    injection_fluid_ = entry.injection_type;
    if (well.verb_vector_[5] > 4) { // idx:5 -> mod
      cout << " injection_fluid_: " << injection_fluid_;
    }
  }
  cout << endl;

  // -------------------------------------------------------
  // Open/Closed
  if (entry.state == ::Settings::Model::WellState::WellOpen) {
    open_ = new Properties::BinaryProperty(true);

  } else if (entry.state == ::Settings::Model::WellState::WellShut) {
    open_ = new Properties::BinaryProperty(false);
  }

  // -------------------------------------------------------
  switch (entry.control_mode) {

    // -----------------------------------------------------
    case ::Settings::Model::ControlMode::BHPControl:

      mode_ = entry.control_mode;
      bhp_ = new Properties::ContinousProperty(entry.bhp);

      // ---------------------------------------------------
      if (well.verb_vector_[5] > 4) // idx:5 -> mod
//        cout << " open_: " << open_->ToString().toStdString()
//             // << " mode_: " << mode_
//             << " bhp_: " << bhp_->value()
//             << endl;

      // ---------------------------------------------------
      if (entry.is_variable) {
        bhp_->setName(entry.name);
        variables->AddVariable(bhp_);
      }
      break;

      // ---------------------------------------------------
    case ::Settings::Model::ControlMode::RateControl:

      mode_ = entry.control_mode;
      rate_ = new Properties::ContinousProperty(entry.rate);

      // ---------------------------------------------------
      if (well.verb_vector_[5] > 4) // idx:5 -> mod
//        cout << " open_: " << open_->ToString().toStdString()
//             // << " mode_: " << mode_
//             << " rate_: " << rate_->value()
//             << endl;

      // ---------------------------------------------------
      if (entry.is_variable) {
        rate_->setName(entry.name);
        variables->AddVariable(rate_);
      }
      break;
  }

}

}
}
