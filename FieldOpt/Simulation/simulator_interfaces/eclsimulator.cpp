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
#include <iostream>

// ---------------------------------------------------------
#include "eclsimulator.h"
#include "Utilities/execution.hpp"
#include "simulator_exceptions.h"
#include "Simulation/results/eclresults.h"

// ---------------------------------------------------------
namespace Simulation {
namespace SimulatorInterfaces {

using namespace Utilities::FileHandling;

// =========================================================
ECLSimulator::ECLSimulator(Settings::Settings *settings,
                           Model::Model *model)
    : Simulator(settings)
{
  model_ = model;
  settings_ = settings;
  UpdateFilePaths();
  deck_name_ = initial_driver_file_name_.split(".DATA").first();

  results_ = new Results::ECLResults();
  try {
    results()->ReadResults(output_driver_file_path_,
                           settings_->verb_vector());
  } catch (...) {}
  // At this stage we don't really care if the results
  // can be read, we just want to set the path.
}

// =========================================================
void ECLSimulator::Evaluate() {

  UpdateFilePaths();
  copyDriverFiles();

  script_args_ = (QStringList() << current_output_deck_parent_dir_path_ << deck_name_);
  auto driver_file_writer = DriverFileWriters::EclDriverFileWriter(settings_, model_);
  if (settings_->verb_vector()[8] > 1) // idx:8 -> sim (Simulation)
    cout << "[sim]script arg[0]:---------- " << script_args_[0].toStdString() << endl
         << "[sim]script arg[1]:---------- " << script_args_[1].toStdString() << endl;


  driver_file_writer.WriteDriverFile(output_schedule_file_path_);
  for (auto well : *model_->wells()) {
    std::cout << well->name().toStdString() << ": " << (well->IsInjector() ? "yes" : "no") << std::endl;
  }

  ::Utilities::Unix::ExecShellScript(script_path_,
                                     script_args_,
                                     settings_->verb_vector());
  results_->ReadResults(current_output_deck_parent_dir_path_ + "/" +initial_driver_file_name_,
                        settings_->verb_vector());
  updateResultsInModel();
}

// =========================================================
bool ECLSimulator::Evaluate(int timeout, int threads) {
  UpdateFilePaths();
  copyDriverFiles();
  script_args_ = (QStringList() << current_output_deck_parent_dir_path_ << deck_name_ << QString::number(threads));
  auto driver_file_writer = DriverFileWriters::EclDriverFileWriter(settings_, model_);
  driver_file_writer.WriteDriverFile(output_schedule_file_path_);
  int t = timeout;
  if (timeout < 10)
    t = 10; // Always let simulations run for at least 10 seconds

  std::cout << "Starting monitored simulation with timeout " << timeout << std::endl;
  bool success = ::Utilities::Unix::ExecShellScriptTimeout(script_path_, script_args_, t);
  std::cout << "Monitored simulation done." << std::endl;
  if (success)
    results_->ReadResults(current_output_deck_parent_dir_path_ + "/" +initial_driver_file_name_);
  updateResultsInModel();
  return success;
}

// =========================================================
void ECLSimulator::CleanUp() {
  QStringList file_endings_to_delete{"DBG", "ECLEND", "ECLRUN", "EGRID", "GRID",
                                     "h5", "INIT","INSPEC", "MSG", "PRT",
                                     "RSSPEC", "UNRST"};
  QString base_file_path = output_driver_file_path_.split(".DATA").first();
  for (QString ending : file_endings_to_delete) {
    DeleteFile(base_file_path + "." + ending);
  }
}

// =========================================================
void ECLSimulator::UpdateFilePaths() {
  current_output_deck_parent_dir_path_ = output_directory_ + "/" + initial_driver_file_parent_dir_name_;
  cout << "output_directory_:" << output_directory_.toStdString() << endl;
  cout << "initial_driver_file_parent_dir_name_:" << initial_driver_file_parent_dir_name_.toStdString() << endl;
  cout << "current_output_deck_parent_dir_path_:" << current_output_deck_parent_dir_path_.toStdString() << endl;

  output_driver_file_path_ = current_output_deck_parent_dir_path_ + "/" + initial_driver_file_name_;
  cout << "current_output_deck_parent_dir_path_:" << current_output_deck_parent_dir_path_.toStdString() << endl;

  cout << "initial_driver_file_name_:" << initial_driver_file_name_.toStdString() << endl;
  cout << "output_driver_file_path_:" << output_driver_file_path_.toStdString() << endl << endl;

//  output_schedule_file_path_ = current_output_deck_parent_dir_path_ + "/" + initial_schedule_path_;
  output_schedule_file_path_ = initial_schedule_path_;
  cout << "output_schedule_file_path_[1]:" << output_schedule_file_path_.toStdString() << endl << endl;

  output_schedule_file_path_.replace(initial_driver_file_parent_dir_path_,
                                     current_output_deck_parent_dir_path_);
  cout << "output_schedule_file_path_[2]:" << output_schedule_file_path_.toStdString() << endl;

}

// =========================================================
void ECLSimulator::WriteDriverFilesOnly() {
  UpdateFilePaths();
  auto driver_file_writer = DriverFileWriters::EclDriverFileWriter(settings_, model_);
  driver_file_writer.WriteDriverFile(output_schedule_file_path_);
}

// =========================================================
void ECLSimulator::copyDriverFiles() {
  if (!DirectoryExists(output_directory_)) {
    std::cout << "Output parent directory does not exist; creating it: "
              << output_directory_.toStdString() << std::endl;
    CreateDirectory(output_directory_);
  }
  if (!DirectoryExists(current_output_deck_parent_dir_path_)) {

    auto input_dir = initial_driver_file_parent_dir_path_ + "/INPUT";
    auto output_dir = current_output_deck_parent_dir_path_ + "/INPUT";

    std::cout << "Output deck directory not found; copying input deck: "
              << "\n" << input_dir.toStdString() << " -> "
              << "\n" << output_dir.toStdString() << std::endl;

    CreateDirectory(output_dir);
    CopyDirectory(input_dir,
                  output_dir,
                  false);

    CopyFile(initial_driver_file_parent_dir_path_ + "/" + initial_driver_file_name_,
             output_driver_file_path_, false);

  }
  assert(DirectoryExists(current_output_deck_parent_dir_path_, true));
  assert(FileExists(output_driver_file_path_, true));
  assert(FileExists(output_schedule_file_path_, true));
}

}
}
