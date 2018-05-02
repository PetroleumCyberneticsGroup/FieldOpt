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
#include "simulator_exceptions.h"

// ---------------------------------------------------------
namespace Simulation {
namespace SimulatorInterfaces {

// ---------------------------------------------------------
using namespace Utilities::FileHandling;
using std::cout;
using std::endl;

// =========================================================
Simulator::Simulator(Settings::Settings *settings) {

  // -------------------------------------------------------
  settings_ = settings;

  // -------------------------------------------------------
  // DRIVER FILE VARIANTS (PATH + NAME)
  initial_driver_file_path_ =
      settings->simulator()->driver_file_path();

  initial_driver_file_name_ =
      initial_driver_file_path_.split("/").last();

  // -------------------------------------------------------
  // PARENT DRIVER FILE VARIANTS (PATH + NAME)
  init_driver_file_parent_dir_path_ =
      settings->simulator()->driver_parent_directory();

  init_driver_file_parent_dir_name_ =
      init_driver_file_parent_dir_path_.split("/").last();

  // -------------------------------------------------------
  // SIM INCLUDE DIR VARIANTS (PATH + NAME)
  init_sim_incl_dir_path_ =
      settings->simulator()->sim_incl_dir_path();

  init_sim_incl_dir_name_ =
      init_sim_incl_dir_path_.split("/").last();

  // -------------------------------------------------------
  initial_schedule_path_ =
      settings->simulator()->schedule_file_path();

  // -------------------------------------------------------
  output_directory_ = settings->output_directory();

  // -------------------------------------------------------
  if (settings->build_path().length() > 0) {
    build_dir_ = settings->build_path() + "/";
    assert(DirectoryExists(build_dir_));
  }

  // -------------------------------------------------------
  // Use custom execution script if provided in runtime
  // settings, else use the one from json driver file
  if (settings->simulator()->
      custom_simulator_execution_script_path().length() > 0) {

    script_path_ =
        settings->simulator()->custom_simulator_execution_script_path();

  } else {
    
    script_path_ = build_dir_ +
        ExecutionScripts::GetScriptPath(settings->simulator()->script_name());
  }

  // -------------------------------------------------------
  script_args_ = (QStringList() << output_directory_
                                << output_directory_ + "/" + initial_driver_file_name_
                                << QString::number(1));

  control_times_ = settings->model()->control_times();

  // -------------------------------------------------------
  auto v = settings_->verb_vector();
  assert(settings->driver_path().length() > 0);

  assert(DirectoryExists(
      init_driver_file_parent_dir_path_, true, v));

  assert(DirectoryExists(output_directory_, true, v));

  assert(FileExists(initial_driver_file_path_, true, v));

  assert(FileExists(initial_schedule_path_, true, v)
             || initial_schedule_path_.length() == 0);

  assert(FileExists(script_path_, true, v));

  // -------------------------------------------------------
  if (v[8] > 1) { // idx:8 -> sim
    cout << "[sim]Simulator set up w/:---- " << endl
         // SIM DRIVER FILE NAME
         << fstr("[sim]init_drvr_file_path_")
         << initial_driver_file_path_.toStdString() << endl
         // SIM DRIVER FILE PATH
         << fstr("[sim]init_drvr_file_name_")
         << initial_driver_file_name_.toStdString() << endl
         // SIM INCLUDE DIR NAME
         << fstr("[sim]init_sim_incl_dir_name_")
         << init_sim_incl_dir_name_.toStdString() << endl
         // SIM EXEC FILE PATH
         << fstr("[sim]script_path_")
         << script_path_.toStdString() << endl
         // SIM EXEC ARG [0]
         << fstr("[sim]script_args_[0]")
         << script_args_.at(0).toStdString() << endl
         // SIM EXEC ARG [1]
         << fstr("[sim]script_args_[1]")
         << script_args_.at(1).toStdString() << endl
         // SIM EXEC ARG [2]
         << fstr("[sim]script_args_[2]")
         << script_args_.at(2).toStdString() << endl
         << endl;
  }
}

// =========================================================
void Simulator::SetOutputDirectory(QString output_directory) {
  if (Utilities::FileHandling::DirectoryExists(output_directory)) {
    output_directory_ = output_directory;
    UpdateFilePaths();
  } else throw OutputDirectoryDoesNotExistException(output_directory);
}

// =========================================================
::Simulation::Results::Results *Simulator::results() {
  return results_;
}

// =========================================================
void Simulator::updateResultsInModel() {
  model_->SetResult("Time", results_->GetValueVector(Results::Results::Property::Time));
  model_->SetResult("Time", results_->GetValueVector(Results::Results::Property::CumulativeGasProduction));
  model_->SetResult("Time", results_->GetValueVector(Results::Results::Property::CumulativeOilProduction));
  model_->SetResult("Time", results_->GetValueVector(Results::Results::Property::CumulativeWaterProduction));
  model_->SetResult("Time", results_->GetValueVector(Results::Results::Property::CumulativeWaterInjection));
}

}
}
