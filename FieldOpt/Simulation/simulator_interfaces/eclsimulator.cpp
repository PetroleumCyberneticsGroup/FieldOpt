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

ECLSimulator::ECLSimulator(Settings::Settings *settings, Model::Model *model)
    : Simulator(settings)
{
    model_ = model;
    driver_file_writer_ = new DriverFileWriters::EclDriverFileWriter(settings, model_);
    script_args_ = (QStringList() << output_directory_
                                  << driver_file_writer_->output_driver_file_name_);

    if (settings_->verb_vector()[8] > 1) // idx:8 -> sim (Simulation)
        cout << "[sim]script arg[0]:--------" << script_args_[0].toStdString() << endl
             << "[sim]script arg[1]:--------" << script_args_[1].toStdString() << endl;

    results_ = new Results::ECLResults();
    try {
        results()->ReadResults(driver_file_writer_->output_driver_file_name_,
                               settings_->verb_vector());
    } catch (...) {}
    // At this stage we don't really care if the results
    // can be read, we just want to set the path.
}

void ECLSimulator::Evaluate()
{
    driver_file_writer_->WriteDriverFile();
    ::Utilities::Unix::ExecShellScript(script_path_, script_args_, settings_->verb_vector());
    results_->ReadResults(driver_file_writer_->output_driver_file_name_,
                          settings_->verb_vector());
    updateResultsInModel();
}

void ECLSimulator::CleanUp()
{
    QStringList file_endings_to_delete{"DBG", "ECLEND", "ECLRUN", "EGRID", "GRID",
                                       "h5", "INIT","INSPEC", "MSG", "PRT",
                                       "RSSPEC", "UNRST"};
    QString base_file_path = driver_file_writer_->output_driver_file_name_.split(".DATA").first();
    for (QString ending : file_endings_to_delete) {
        Utilities::FileHandling::DeleteFile(base_file_path + "." + ending);
    }
}

void ECLSimulator::UpdateFilePaths()
{
    return;
}

bool ECLSimulator::Evaluate(int timeout, int threads) {
    int t = timeout;
    if (timeout < 10) t = 10; // Always let simulations run for at least 10 seconds
    script_args_[2] = QString::number(threads);

    driver_file_writer_->WriteDriverFile();
    std::cout << "Starting monitored simulation with timeout " << timeout << std::endl;
    bool success = ::Utilities::Unix::ExecShellScriptTimeout(script_path_, script_args_, t);
    std::cout << "Monitored simulation done." << std::endl;
    if (success) results_->ReadResults(driver_file_writer_->output_driver_file_name_);
    updateResultsInModel();
    return success;
}
void ECLSimulator::WriteDriverFilesOnly() {
    driver_file_writer_->WriteDriverFile();
}

}
}
