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

// -----------------------------------------------------------------
#include <iostream>

#include "adgprssimulator.h"
#include "Utilities/execution.hpp"
#include "simulator_exceptions.h"
#include "Simulation/results/adgprsresults.h"

// -----------------------------------------------------------------
using std::cout;

// -----------------------------------------------------------------
namespace Simulation {
namespace SimulatorInterfaces {

// -----------------------------------------------------------------
AdgprsSimulator::AdgprsSimulator(Settings::Settings *settings,
                                 Model::Model *model)
    : Simulator(settings) {

  // ---------------------------------------------------------------
  // GPRS.txt
  QStringList tmp = initial_driver_file_path_.split("/");
  tmp.removeLast();
  initial_driver_file_parent_dir_path_ = tmp.join("/");
  verifyOriginalDriverFileDirectory();

  // ---------------------------------------------------------------
  // OPT.txt
  tmp = initial_driver_file_path_.split("___");
  initial_driver_file_name_opt_ = "OPT___" + tmp.last();
  initial_driver_file_path_opt_ =
      initial_driver_file_parent_dir_path_ + "/" + initial_driver_file_name_opt_;

  // ---------------------------------------------------------------
  // H5 output file
  output_h5_summary_file_path_ =
      output_directory_ + "/" + initial_driver_file_name_.split(".").first() + ".vars.h5";

  // ---------------------------------------------------------------
  model_ = model;
  results_ = new Simulation::Results::AdgprsResults();

  driver_file_writer_ = new DriverFileWriters::AdgprsDriverFileWriter(settings_, model_);
}

// -----------------------------------------------------------------
void AdgprsSimulator::Evaluate() {

  if (results_->isAvailable()) results()->DumpResults();

  copyDriverFiles();
  driver_file_writer_->WriteDriverFile(output_directory_);
  ::Utilities::Unix::ExecShellScript(script_path_,
                                     script_args_,
                                     settings_->verb_vector());
  results_->ReadResults(output_h5_summary_file_path_);
  updateResultsInModel();
}

// -----------------------------------------------------------------
void AdgprsSimulator::CleanUp() {
  Utilities::FileHandling::DeleteFile(output_h5_summary_file_path_);
}

// -----------------------------------------------------------------
void AdgprsSimulator::copyDriverFiles() {

  // Copy ----------------------------------------------------------
  auto init_file_path = initial_driver_file_path_;
  auto run_file_name = output_directory_ + "/" + initial_driver_file_name_;
  Utilities::FileHandling::CopyFile(init_file_path,
                                    run_file_name,
                                    true);

  // Dbg -----------------------------------------------------------
  if (settings_->verb_vector()[8] > 1) { // idx:8 -> sim (Simulation)
    cout << "[sim]init_drvr_file_path:---- "
         << init_file_path.toStdString() << endl;
    cout << "[sim]run_drvr_file:---------- "
         << run_file_name.toStdString() << endl;
  }

  // Copy ----------------------------------------------------------
  auto init_file_path_opt = initial_driver_file_path_opt_;
  auto run_file_name_opt = output_directory_ + "/" + initial_driver_file_name_opt_;
  Utilities::FileHandling::CopyFile(init_file_path_opt,
                                    run_file_name_opt,
                                    true);

  // Dbg -----------------------------------------------------------
  if (settings_->verb_vector()[8] > 1) { // idx:8 -> sim (Simulation)
    cout << "[sim]init_drvr_file_path_opt: "
         << init_file_path_opt.toStdString() << endl;
    cout << "[sim]run_drvr_file_opt:------ "
         << run_file_name_opt.toStdString() << endl;
  }

  // ---------------------------------------------------------------
  Utilities::FileHandling::CreateDirectory(output_directory_ + "/include");
  Utilities::FileHandling::CopyDirectory(initial_driver_file_parent_dir_path_ + "/include",
                                         output_directory_ + "/include");

  // ---------------------------------------------------------------
  Utilities::FileHandling::CreateDirectory(output_directory_ + "/INPUT");
  Utilities::FileHandling::CopyDirectory(initial_driver_file_parent_dir_path_ + "/INPUT",
                                         output_directory_ + "/INPUT");
}

// -----------------------------------------------------------------
void AdgprsSimulator::verifyOriginalDriverFileDirectory() {

  QStringList critical_files = {initial_driver_file_path_,
                                initial_driver_file_parent_dir_path_ + "/include/compdat.in",
                                initial_driver_file_parent_dir_path_ + "/include/controls.in",
                                initial_driver_file_parent_dir_path_ + "/include/wells.in",
                                initial_driver_file_parent_dir_path_ + "/include/welspecs.in"};
  for (auto file : critical_files) {
    if (!Utilities::FileHandling::FileExists(file))
      throw DriverFileDoesNotExistException(file);
  }
}

// -----------------------------------------------------------------
void AdgprsSimulator::UpdateFilePaths() {

  output_h5_summary_file_path_ = output_directory_ + "/" +
      initial_driver_file_name_.split(".").first() + ".vars.h5";
  script_args_ = (QStringList() << output_directory_
                                << output_directory_ + "/" + initial_driver_file_name_);
}

// -----------------------------------------------------------------
bool AdgprsSimulator::Evaluate(int timeout, int threads) {

  // ---------------------------------------------------------------
  script_args_[2] = QString::number(threads);
  int t = timeout;
  if (timeout < 10) t = 10; // Always let simulations run for at least 10 seconds
  if (results_->isAvailable()) results()->DumpResults();
  copyDriverFiles();

  // ---------------------------------------------------------------
  driver_file_writer_->WriteDriverFile(output_directory_);

  if (settings_->verb_vector()[8] > 1) { // idx:8 -> sim (Simulation)
    cout << "[sim]Start.sim w/ timeout:--- " << timeout << endl;
    cout << "[sim]script arg[0]:---------- " << script_args_[0].toStdString() << endl
         << "[sim]script arg[1]:---------- " << script_args_[1].toStdString() << endl;
  }

  bool success = ::Utilities::Unix::ExecShellScriptTimeout(script_path_,
                                                           script_args_,
                                                           t);
  if (success) {
    results_->ReadResults(output_h5_summary_file_path_);
  }
  updateResultsInModel();
  return success;
}

// -----------------------------------------------------------------
void AdgprsSimulator::WriteDriverFilesOnly() {
  copyDriverFiles();
  driver_file_writer_->WriteDriverFile(output_directory_);
}

}
}
