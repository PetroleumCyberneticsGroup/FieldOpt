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
#include "simulator_exceptions.h"
#include "Simulation/results/eclresults.h"
#include "Utilities/execution.hpp"
#include "Utilities/debug.hpp"

// ---------------------------------------------------------
namespace Simulation {
namespace SimulatorInterfaces {

// ---------------------------------------------------------
using namespace Utilities::FileHandling;
using std::cout;
using std::endl;

// =========================================================
ECLSimulator::ECLSimulator(Settings::Settings *settings,
                           Model::Model *model)
    : Simulator(settings) {

  // ---------------------------------------------------------
  model_ = model;
  settings_ = settings;
  UpdateFilePaths();
  deck_name_ = initial_driver_file_name_.split(".DATA").first();

  // ---------------------------------------------------------
  results_ = new Results::ECLResults();

  // ---------------------------------------------------------
  try {
    results()->ReadResults(output_driver_file_path_,
                           settings_->verb_vector());
  } catch (...) {}
  // At this stage we don't really care if the results
  // can be read, we just want to set the path.
}

// =========================================================
void ECLSimulator::Evaluate() {

  // -------------------------------------------------------
  UpdateFilePaths();
  copyDriverFiles();

  // -------------------------------------------------------
  script_args_ =
      (QStringList() << current_output_deck_parent_dir_path_
                     << deck_name_);

  // -------------------------------------------------------
  auto driver_file_writer =
      DriverFileWriters::EclDriverFileWriter(settings_, model_);

  // -------------------------------------------------------
  if (settings_->verb_vector()[8] > 1) // idx:8 -> sim (Simulation)
    cout << "[sim]script arg[0]:---------- "
         << script_args_[0].toStdString() << endl
         << "[sim]script arg[1]:---------- "
         << script_args_[1].toStdString() << endl;

  // -------------------------------------------------------
  driver_file_writer.WriteDriverFile(output_schedule_file_path_);

  // -------------------------------------------------------
  for (auto well : *model_->wells()) {
    cout << well->name().toStdString() << ": "
         << (well->IsInjector() ? "yes" : "no") << std::endl;
  }

  // -------------------------------------------------------
  ::Utilities::Unix::ExecShellScript(script_path_,
                                     script_args_,
                                     settings_->verb_vector());

  // -------------------------------------------------------
  results_->ReadResults(current_output_deck_parent_dir_path_
                            + "/" +initial_driver_file_name_,
                        settings_->verb_vector());

  // -------------------------------------------------------
  updateResultsInModel();
}

// =========================================================
bool ECLSimulator::Evaluate(int timeout, int threads) {

  // -------------------------------------------------------
  UpdateFilePaths();
  copyDriverFiles();

  // -------------------------------------------------------
  script_args_ =
      (QStringList()
          << current_output_deck_parent_dir_path_
          << deck_name_
          << QString::number(threads));

  // -------------------------------------------------------
  auto driver_file_writer =
      DriverFileWriters::EclDriverFileWriter(settings_, model_);

  // -------------------------------------------------------
  driver_file_writer.WriteDriverFile(output_schedule_file_path_);

  // -------------------------------------------------------
  int t = timeout;
  if (timeout < 10)
    t = 10; // Always let simulations run for at least 10 seconds

  // -------------------------------------------------------
  cout << "Starting monitored simulation with timeout "
       << timeout << endl;

  // -------------------------------------------------------
  bool success =
      ::Utilities::Unix::ExecShellScriptTimeout(script_path_,
                                                script_args_,
                                                t);
  cout << "Monitored simulation done." << endl;

  // -------------------------------------------------------
  if (success)
    results_->ReadResults(
        current_output_deck_parent_dir_path_
            + "/" +initial_driver_file_name_);

  // -------------------------------------------------------
  updateResultsInModel();
  return success;
}

// =========================================================
void ECLSimulator::CleanUp() {

  // -------------------------------------------------------
  QStringList file_endings_to_delete{
      "DBG", "ECLEND", "ECLRUN", "EGRID", "GRID",
      "h5", "INIT","INSPEC", "MSG", "PRT",
      "RSSPEC", "UNRST"};

  // -------------------------------------------------------
  QString base_file_path =
      output_driver_file_path_.split(".DATA").first();

  // -------------------------------------------------------
  for (QString ending : file_endings_to_delete) {
    DeleteFile(base_file_path + "." + ending);
  }
}

// =========================================================
void ECLSimulator::UpdateFilePaths() {

  // -------------------------------------------------------
  current_output_deck_parent_dir_path_ =
      output_directory_ + "/" + init_driver_file_parent_dir_name_;

  // -------------------------------------------------------
  if (settings_->verb_vector()[8] > 3) { // idx:8 -> sim

    cout << fstr("[sim]Updating file paths",8) << endl;
    // OUTPUT DIRECTORY
    cout << fstr("outp_directory_",8)
         << output_directory_.toStdString() << endl;
    // CURRENT OUTPUT DECK PARENT DIR PATH
    cout << fstr("curr_outp_dck_prnt_dir_pth_",8)
         << current_output_deck_parent_dir_path_.toStdString() << endl;
    // INITIAL DRIVER FILE PARENT DIR NAME
    cout << fstr("init_drvr_fl_prnt_dir_pth_",8)
         << init_driver_file_parent_dir_path_.toStdString() << endl;
    // INITIAL DRIVER FILE PARENT DIR NAME
    cout << fstr("init_drvr_fl_prnt_dir_nm_",8)
         << init_driver_file_parent_dir_name_.toStdString() << endl;

  }

  // E.g.:

  // OUTPUT DIRECTORY:
  // -------->  /xruns/xruns___EEE/output-runs/rank0

  // CURRENT OUTPUT DECK PARENT DIR PATH:
  // -------->  /xruns/xruns___EEE/output-runs/rank0/xruns___EEE

  // INITIAL DRIVER FILE PARENT DIR PATH:
  // -------->

  // INITIAL DRIVER FILE PARENT DIR NAME:
  // -------->  xruns___EEE

  // -------------------------------------------------------
  output_driver_file_path_ =
      current_output_deck_parent_dir_path_
          + "/" + initial_driver_file_name_;

  // -------------------------------------------------------
  output_sim_incl_dir_name_ = init_sim_incl_dir_name_;

  // -------------------------------------------------------
  output_schedule_file_path_ = initial_schedule_path_;

  // -------------------------------------------------------
  if (settings_->verb_vector()[8] > 3) { // idx:8 -> sim

    // INITIAL DRIVER FILE NAME
    cout << fstr("init_drvr_fl_nm_",8)
         << initial_driver_file_name_.toStdString() << endl;
    // OUTPUT DRIVER FILE PATH
    cout << fstr("outp_drvr_fl_pth_",8)
         << output_driver_file_path_.toStdString() << endl;
    // OUTPUT SCHEDULE FILE PATH [0]
    cout << fstr("outp_schdl_fl_pth_[0]",8)
         << output_schedule_file_path_.toStdString() << endl;

  }

  // E.g.:

  // INITIAL DRIVER FILE NAME
  // -------->  MODEL_DOT.DATA

  // OUTPUT DRIVER FILE PATH
  // -------->  /xruns/xruns___EEE/output-runs/rank0/xruns___EEE/MODEL_DOT.DATA

  // OUTPUT SCHEDULE FILE PATH [0]
  // -------->  ../xruns___EEE/./INPUT/OLYMPUS_SCH.INC

  // -------------------------------------------------------
  output_schedule_file_path_.replace(init_driver_file_parent_dir_path_,
                                     current_output_deck_parent_dir_path_);

  // -------------------------------------------------------
  if (settings_->verb_vector()[8] > 3) { // idx:8 -> sim

    // OUTPUT SCHEDULE FILE PATH [1]
    cout << fstr("outp_schdl_fl_pth_[1]",8)
         << output_schedule_file_path_.toStdString() << endl;

  }
}

// E.g.:

// OUTPUT SCHEDULE FILE PATH [1]
// -------->  /xruns/xruns___EEE/output-runs/rank0//xruns___EEE/./INPUT/OLYMPUS_SCH.INC

// =========================================================
void ECLSimulator::WriteDriverFilesOnly() {

  // -------------------------------------------------------
  UpdateFilePaths();

  // -------------------------------------------------------
  auto driver_file_writer =
      DriverFileWriters::EclDriverFileWriter(settings_, model_);

  // -------------------------------------------------------
  driver_file_writer.WriteDriverFile(output_schedule_file_path_);
}

// =========================================================
void ECLSimulator::copyDriverFiles() {

  // -------------------------------------------------------
  if (!DirectoryExists(output_directory_)
      && settings_->verb_vector()[8] > 3) {

    cout << fstr("Output parent dir does not exist; creating it",8)
         << output_directory_.toStdString() << endl;
    CreateDirectory(output_directory_);

  }

  // -------------------------------------------------------
  if (!DirectoryExists(current_output_deck_parent_dir_path_)) {

    // -----------------------------------------------------
    auto input_dir =
        init_driver_file_parent_dir_path_
            + "/" + output_sim_incl_dir_name_;

    // -----------------------------------------------------
    auto output_dir =
        current_output_deck_parent_dir_path_
            + "/" + output_sim_incl_dir_name_;

    // -----------------------------------------------------
    if (settings_->verb_vector()[8] > 3) { // idx:8 -> sim

      cout << fstr("Output deck dir not found; copying input deck",8)
           << output_schedule_file_path_.toStdString() << endl;
      cout << FLBLUE << "FROM: " << input_dir.toStdString() << AEND << endl
           << FLBLUE << "TO:   " << output_dir.toStdString() << AEND << endl;

    }

    // E.g.:

    // input_dir:
    // -------->  ../xruns___EEE/./INPUT

    // output_dir:
    // -------->  /xruns/xruns___EEE/output-runs/rank0//xruns___EEE/INPUT

    // -----------------------------------------------------
    CreateDirectory(output_dir);

    // -----------------------------------------------------
    CopyDirectory(input_dir,
                  output_dir,
                  false);

    // -----------------------------------------------------
    auto input_file = init_driver_file_parent_dir_path_
        + "/" + initial_driver_file_name_;

    // -----------------------------------------------------
    auto output_file = output_driver_file_path_;

    // E.g.:

    // input_file:
    // -------->

    // output_file:
    // -------->  /xruns/xruns___EEE/output-runs/rank0/xruns___EEE/MODEL_DOT.DATA

    // -----------------------------------------------------
    CopyFile(input_file,
             output_file,
             false);

  }

  // -------------------------------------------------------
  assert(DirectoryExists(current_output_deck_parent_dir_path_, true));
  assert(FileExists(output_driver_file_path_, true));
  assert(FileExists(output_schedule_file_path_, true));
}

}
}
