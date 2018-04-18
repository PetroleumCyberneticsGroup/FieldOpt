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

#ifndef SETTINGS_SIMULATOR_H
#define SETTINGS_SIMULATOR_H

#include "settings.h"

#include <QStringList>

namespace Settings {

/*!
 * \brief The Simulator class contains simulator-specific settings. Simulator settings objects
 * may _only_ be created by the Settings class. They are created when reading a JSON-formatted
 * "driver file".
 */
class Simulator
{
  friend class Settings;

 public:
  enum SimulatorType { ECLIPSE, ADGPRS, Flow };
  enum SimulatorFluidModel { BlackOil, DeadOil };


  /*!
   * Get the simulator type (e.g. ECLIPSE).
   */
  SimulatorType type() const { return type_; }

  /*!
   * Get the simulator commands (commands used to execute a simulation).
   * Each list element is executed in sequence.
   */
  QStringList *commands() const { return commands_; }

  /*!
   * Get the name of the script to be used to execute simulations.
   */
  QString script_name() const { return script_name_; }

  /*!
   * Get the path to the driver file.
   */
  QString driver_file_path() const { return driver_file_path_; }

  /*!
   * Get the path to the file containing the schedule section
   (or the part of it that is to be manipulated).
   */
  QString schedule_file_path() const {return schedule_file_path_; }

  /*!
   * Set the driver file path. Used when the path is passed by command line argument.
   */
  void set_driver_file_path(const QString path) { driver_file_path_ = path; }

  /*!
   * Set the execution script path.
   */
  void set_execution_script_path (const QString path) { custom_exec_script_path_ = path; }

  /*!
   * Set a custom path for the simulator execution script.
   */

  /*!
   * Get the path to the simulator execution script.
   */
  QString custom_simulator_execution_script_path() const { return custom_exec_script_path_; }

  /*!
   * Get the output directory path.
   */
  QString output_directory() const { return output_directory_; }

  /*!
   * Get the fluid model.
   */
  SimulatorFluidModel fluid_model() const { return fluid_model_; }


  /*!
   * Get the maximum number of minutes simulations are allowed to run
   if no timeout value can be calculated. Returns -1 if field is not set.
   */
  int max_minutes() { return max_minutes_; }

 private:
  Simulator(QJsonObject json_simulator);
  SimulatorType type_;
  SimulatorFluidModel fluid_model_;
  QStringList *commands_;
  QString script_name_;
  QString driver_file_path_;
  QString schedule_file_path_;
  QString output_directory_;
  QString custom_exec_script_path_;
  int max_minutes_;
};

}


#endif // SETTINGS_SIMULATOR_H
