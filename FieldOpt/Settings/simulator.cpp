/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include "simulator.h"
#include "settings_exceptions.h"
#include "Utilities/filehandling.hpp"
#include "Utilities/debug.hpp"

// ---------------------------------------------------------

// ---------------------------------------------------------
namespace Settings {

// =========================================================
Simulator::Simulator(QJsonObject json_simulator) {

  // -------------------------------------------------------
  // Driver path
  if (json_simulator.contains("DriverPath"))
    driver_file_path_ = json_simulator["DriverPath"].toString();
  else driver_file_path_ = "";

  // -------------------------------------------------------
  // Simulator type
  QString type = json_simulator["Type"].toString();
  if (QString::compare(type, "ECLIPSE") == 0) {
    type_ = SimulatorType::ECLIPSE;

  } else if (QString::compare(type, "ADGPRS") == 0) {
    type_ = SimulatorType::ADGPRS;

  } else if (QString::compare(
      type, "Flow", Qt::CaseInsensitive) == 0) {
    type_ = SimulatorType::Flow;

  } else {
    throw SimulatorTypeNotRecognizedException(
        "The simulator type " + type.toStdString()
            + " was not recognized");
  }

  // -------------------------------------------------------
  // Maximum runtime
  if (json_simulator.contains("MaxMinutes")
      && json_simulator["MaxMinutes"].toInt() > 0) {
    max_minutes_ = json_simulator["MaxMinutes"].toInt();

  } else {
    max_minutes_ = -1;
  }

  // -------------------------------------------------------
  // Simulator commands
  QJsonArray commands = json_simulator["Commands"].toArray();
  script_name_ = "";

  // -------------------------------------------------------
  if (json_simulator.contains("ExecutionScript")
      && json_simulator["ExecutionScript"].toString().size() > 0) {
    script_name_ = json_simulator["ExecutionScript"].toString();
  }

  // -------------------------------------------------------
  if (json_simulator.contains("Commands") && commands.size() > 0) {
    commands_ = new QStringList();
    for (int i = 0; i < commands.size(); ++i) {
      commands_->append(commands[i].toString());
    }
  }
  if (script_name_.length() == 0 && commands.size() == 0)
    throw NoSimulatorCommandsGivenException("At least one simulator command or a simulator script must be given.");

  if (json_simulator.contains("FluidModel")) {
    QString fluid_model = json_simulator["FluidModel"].toString();
    if (QString::compare(fluid_model, "DeadOil") == 0)
      fluid_model_ = SimulatorFluidModel::DeadOil;
    else if (QString::compare(fluid_model, "BlackOil") == 0)
      fluid_model_ = SimulatorFluidModel::BlackOil;
  }
  else fluid_model_ = SimulatorFluidModel::BlackOil;
}

// =========================================================
void Simulator::set_schedule_file_path(const QString path) {
  schedule_file_path_ = path;
  cout << fstr("Override DRV with RTS",9)
       << schedule_file_path_.toStdString() << endl;
}

// =========================================================
void Simulator::set_sim_incl_dir_path(const QString path) {
  sim_incl_dir_path_ = path;
  cout << fstr("Override DRV with RTS",9)
       << sim_incl_dir_path_.toStdString() << endl;
}

// =========================================================
void Simulator::set_driver_file_path(const QString path) {

  // -------------------------------------------------------
  driver_file_path_ = path;
  cout << fstr("Override DRV with RTS",9)
       << driver_file_path_.toStdString() << endl;

  // -------------------------------------------------------
  if (!FileExists(driver_file_path_)) {
    throw std::runtime_error(
        "No file found at DriverPath: "
            + driver_file_path_.toStdString());
  }

  // -------------------------------------------------------
  auto old_driver_directory = driver_directory_;
  auto tmp = driver_file_path_.split("/");
  tmp.removeLast();
  driver_directory_ = tmp.join("/");

  // -------------------------------------------------------
  std::cout << "sched_file_path_:"
            << schedule_file_path_.toStdString()
            << std::endl;

  // -------------------------------------------------------
  if (schedule_file_path_.length() > 0) {
    schedule_file_path_.replace(old_driver_directory,
                                driver_directory_);

    // -----------------------------------------------------
    if (!FileExists(schedule_file_path_))
      throw std::runtime_error(
          "No file found at ScheduleFile: "
              + schedule_file_path_.toStdString());
  }
}

}
