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
#include <iostream>
#include "wellcontrols.h"

// ---------------------------------------------------------
namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {

// =========================================================
WellControls::WellControls(QList<Model::Wells::Well *> *wells,
                           QList<double> control_times) {
  initializeBaseEntryLine(12);
  initializeTimeEntries(wells,
                        control_times);
}

// =========================================================
QString WellControls::GetPartString() {

  // -------------------------------------------------------
  QString part = "";
  double prev_time = 0;

  // -------------------------------------------------------
  for (double time : time_entries_.keys()) {
    part.append(createTimeEntry(time, prev_time));
    prev_time = time;

    // -----------------------------------------------------
    if (time_entries_[time]->has_well_setting) {

      // ---------------------------------------------------
      for (WellSetting *setting :
          time_entries_.value(time)->well_settings) {

        // -------------------------------------------------
        if (setting->is_injector) {
          part.append(createInjectorEntry(setting));
        } else {
          part.append(createProducerEntry(setting));
        }
      }

    }
  }
  return part;
}

// =========================================================
void WellControls::
initializeTimeEntries(QList<Model::Wells::Well *> *wells,
                      QList<double> control_times) {

  // -------------------------------------------------------
  time_entries_ = QMap<int, TimeEntry *>();

  // -------------------------------------------------------
  for (int i = 0; i < wells->size(); ++i) {

    // -----------------------------------------------------
    if (wells->at(i)->trajectory()->GetWellBlocks()->size() > 0) {

      // ---------------------------------------------------
      for (int j = 0; j < wells->at(i)->controls()->size(); ++j) {

        // -------------------------------------------------
        int current_time_step =
            wells->at(i)->controls()->at(j)->time_step();

        // -------------------------------------------------
        WellSetting *well_setting = new WellSetting();
        well_setting->well_name = wells->at(i)->name();

        // -------------------------------------------------
        well_setting->is_injector =
            wells->at(i)->type() == ::Settings::Model::WellType::Injector;

        // -------------------------------------------------
        well_setting->control = wells->at(i)->controls()->at(j);

        // -------------------------------------------------
        if (time_entries_.keys().contains(current_time_step)) {

          // -----------------------------------------------
          // Adding to existing time step
          time_entries_.value(current_time_step)->
              well_settings.append(well_setting);

        } else {

          // -----------------------------------------------
          // Adding new time step
          TimeEntry *time_entry = new TimeEntry();
          time_entry->time = current_time_step;

          time_entry->well_settings =
              QList<WellSetting *>({well_setting});

          time_entry->has_well_setting = true;
          time_entries_.insert(time_entry->time,
                               time_entry);

        }

      }
    }
  }

  // -------------------------------------------------------
  for (int t = 0; t < control_times.size(); ++t) {

    // -----------------------------------------------------
    // Adding a control time without a well setting
    if (!time_entries_.contains(control_times[t])) {

      // ---------------------------------------------------
      auto control_time_entry = new TimeEntry();
      control_time_entry->time = control_times[t];

      // -----------------------------------------------------
      time_entries_.insert(
          control_time_entry->time, control_time_entry);

    }
  }
}

// =========================================================
QString WellControls::createTimeEntry(double time,
                                      double prev_time) {

  // -------------------------------------------------------
  // A Time entry should not be created for the initial step
  if (time == 0) {
    return QString("");
  }

  // -------------------------------------------------------
  // The amount of time to advance
  double delta_time = time - prev_time;
  return QString("TSTEP\n%1 /\n\n").arg(delta_time);
}

// =========================================================
QString WellControls::
createProducerEntry(WellControls::WellSetting *setting) {

  // -------------------------------------------------------
  initializeBaseEntryLine(9);
  QStringList producer_entry_line = QStringList(base_entry_line_);
  producer_entry_line[0] = setting->well_name;

  // -------------------------------------------------------
  if (setting->control->open()) {
    producer_entry_line[1] = "OPEN";
  } else {
    producer_entry_line[1] = "SHUT";
  }

  // -------------------------------------------------------
  switch (setting->control->mode()) {

    case ::Settings::Model::ControlMode::BHPControl:
      producer_entry_line[2] = "BHP";
      producer_entry_line[8] =
          QString::number(setting->control->bhp());
      break;

    case ::Settings::Model::ControlMode::RateControl:
      producer_entry_line[2] = "LRAT";
      producer_entry_line[6] =
          QString::number(setting->control->rate());
      break;

    default:
      throw std::runtime_error(
          "Producer control mode not recognized.");
  }

  return "WCONPROD\n   "
      + producer_entry_line.join(" ") + "/\n/\n\n";
}

// =========================================================
QString WellControls::
createInjectorEntry(WellControls::WellSetting *setting) {

  // -------------------------------------------------------
  initializeBaseEntryLine(7);
  QStringList injector_entry_line = QStringList(base_entry_line_);
  injector_entry_line[0] = setting->well_name;

  // -------------------------------------------------------
  switch (setting->control->injection_fluid()) {

    case ::Settings::Model::InjectionType::WaterInjection:
      injector_entry_line[1] = "WATER";
      break;

    case ::Settings::Model::InjectionType::GasInjection:
      injector_entry_line[1] = "GAS";
      break;

    default:
      cout << "injector_entry_line[0]: "
           << injector_entry_line[0].toStdString() << endl
           << "injector_entry_line[1]:"
           << setting->control->injection_fluid() << endl;

      throw std::runtime_error("Injector type not recognized.");
  }

  // -------------------------------------------------------
  if (setting->control->open()) {
    injector_entry_line[2] = "OPEN";

  } else {
    injector_entry_line[2] = "SHUT";
  }

  // -------------------------------------------------------
  switch (setting->control->mode()) {

    case ::Settings::Model::ControlMode::BHPControl:
      injector_entry_line[3] = "BHP";
      injector_entry_line[6] = QString::number(setting->control->bhp());
      break;

    case ::Settings::Model::ControlMode::RateControl:
      injector_entry_line[3] = "RATE";
      injector_entry_line[4] = QString::number(setting->control->rate());
      break;

    default:
      throw std::runtime_error("Producer control mode not recognized.");
  }
  return "WCONINJE\n   " + injector_entry_line.join(" ") + "/\n/\n\n";
}



}
}
}
}
}
