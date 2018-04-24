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
#include <iostream>
#include "eclsimulator.h"
#include "Utilities/execution.hpp"
#include "simulator_exceptions.h"
#include "Simulation/results/eclresults.h"

namespace Simulation {
namespace SimulatorInterfaces {

using namespace Utilities::FileHandling;

ECLSimulator::ECLSimulator(Settings::Settings *settings, Model::Model *model)
    : Simulator(settings)
{
    model_ = model;
    driver_file_writer_ = new DriverFileWriters::EclDriverFileWriter(settings, model_);
    UpdateFilePaths();
    deck_name_ = initial_driver_file_name_.split(".DATA").first();

    results_ = new Results::ECLResults();
    try {
        results()->ReadResults(output_driver_file_path_);
    } catch (...) {} // At this stage we don't really care if the results can be read, we just want to set the path.
}

void ECLSimulator::Evaluate()
{
    UpdateFilePaths();
    copyDriverFiles();
    script_args_ = (QStringList() << current_output_deck_parent_dir_path_ << deck_name_);
    driver_file_writer_->WriteDriverFile(output_schedule_file_path_);
    std::cout << "Well is injector? " << std::endl;
    for (auto well : *model_->wells()) {
        std::cout << well->name().toStdString() << ": " << (well->IsInjector() ? "yes" : "no") << std::endl;
    }
    ::Utilities::Unix::ExecShellScript(script_path_, script_args_);
    results_->ReadResults(current_output_deck_parent_dir_path_ + "/" +initial_driver_file_name_);
    updateResultsInModel();
}

bool ECLSimulator::Evaluate(int timeout, int threads) {
    UpdateFilePaths();
    copyDriverFiles();
    script_args_ = (QStringList() << current_output_deck_parent_dir_path_ << deck_name_ << QString::number(threads));
    driver_file_writer_->WriteDriverFile(output_schedule_file_path_);
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

void ECLSimulator::CleanUp()
{
    UpdateFilePaths();
    QStringList file_endings_to_delete{"DBG", "ECLEND", "ECLRUN", "EGRID", "GRID",
                                       "h5", "INIT","INSPEC", "MSG", "PRT",
                                       "RSSPEC", "UNRST"};
    QString base_file_path = output_driver_file_path_.split(".DATA").first();

    for (QString ending : file_endings_to_delete) {
        DeleteFile(base_file_path + "." + ending);
    }
}

void ECLSimulator::UpdateFilePaths()
{
    current_output_deck_parent_dir_path_ = output_directory_ + "/" + initial_driver_file_parent_dir_name_;
    output_driver_file_path_ = current_output_deck_parent_dir_path_ + "/" + initial_driver_file_name_;
    output_schedule_file_path_ = initial_schedule_path_;
    output_schedule_file_path_.replace(initial_driver_file_parent_dir_path_,current_output_deck_parent_dir_path_);
}

void ECLSimulator::WriteDriverFilesOnly() {
    UpdateFilePaths();
    driver_file_writer_->WriteDriverFile(current_output_deck_parent_dir_path_);
}

void ECLSimulator::copyDriverFiles() {
    if (!DirectoryExists(output_directory_)) {
        std::cout << "Output parent directory does not exist; creating it: " << output_directory_.toStdString() << std::endl;
        CreateDirectory(output_directory_);
    }
    if (!DirectoryExists(current_output_deck_parent_dir_path_)) {
        std::cout << "Output deck directory not found; copying input deck: "
                  << "\t" << initial_driver_file_parent_dir_path_.toStdString() << " -> "
                  << "\t" << current_output_deck_parent_dir_path_.toStdString() << std::endl;
        CreateDirectory(current_output_deck_parent_dir_path_);
        CopyDirectory(initial_driver_file_parent_dir_path_, current_output_deck_parent_dir_path_, true);
    }
}

}
}
