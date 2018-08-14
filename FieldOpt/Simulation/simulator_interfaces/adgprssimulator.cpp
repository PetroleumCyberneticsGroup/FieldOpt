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
#include "adgprssimulator.h"
#include "Utilities/execution.hpp"
#include "simulator_exceptions.h"
#include "Simulation/results/adgprsresults.h"

namespace Simulation {
namespace SimulatorInterfaces {

AdgprsSimulator::AdgprsSimulator(Settings::Settings *settings, Model::Model *model)
    : Simulator(settings)
{
    verifyOriginalDriverFileDirectory();

    model_ = model;
    results_ = new Simulation::Results::AdgprsResults();
    driver_file_writer_ = new DriverFileWriters::AdgprsDriverFileWriter(settings_, model_);
}

void AdgprsSimulator::Evaluate()
{
    if (results_->isAvailable()) results()->DumpResults();
    copyDriverFiles();
    driver_file_writer_->WriteDriverFile(QString::fromStdString(paths_.GetPath(Paths::SIM_WORK_DIR)));
    ::Utilities::Unix::ExecShellScript(QString::fromStdString(paths_.GetPath(Paths::SIM_EXEC_SCRIPT_FILE)), script_args_);
    paths_.SetPath(Paths::SIM_HDF5_FILE,
        paths_.GetPath(Paths::SIM_WORK_DIR) + "/"
        + driver_file_name_.split(".").first().toStdString() + ".vars.h5"
    );
    results_->ReadResults(QString::fromStdString(paths_.GetPath(Paths::SIM_HDF5_FILE)));
    updateResultsInModel();
}

void AdgprsSimulator::CleanUp()
{
    DeleteFile(QString::fromStdString(paths_.GetPath(Paths::SIM_HDF5_FILE)));
}

void AdgprsSimulator::copyDriverFiles()
{
    auto workdir = paths_.GetPath(Paths::OUTPUT_DIR) + driver_parent_dir_name_.toStdString();
    if (!DirectoryExists(workdir)) {
        std::cout << "Output deck directory not found; copying input deck: "
                  << "\t" << paths_.GetPath(Paths::SIM_DRIVER_DIR) << " -> "
                  << "\t" << workdir << std::endl;
        CreateDirectory(workdir);
        CopyDirectory(paths_.GetPath(Paths::SIM_DRIVER_DIR), workdir, true);
    }
    paths_.SetPath(Paths::SIM_WORK_DIR, workdir);
}

void AdgprsSimulator::verifyOriginalDriverFileDirectory()
{
    QStringList critical_files = {QString::fromStdString(paths_.GetPath(Paths::SIM_DRIVER_DIR)) + "/include/compdat.in",
                                  QString::fromStdString(paths_.GetPath(Paths::SIM_DRIVER_DIR)) + "/include/controls.in",
                                  QString::fromStdString(paths_.GetPath(Paths::SIM_DRIVER_DIR)) + "/include/wells.in",
                                  QString::fromStdString(paths_.GetPath(Paths::SIM_DRIVER_DIR)) + "/include/welspecs.in"};
    for (auto file : critical_files) {
        if (!Utilities::FileHandling::FileExists(file))
            throw DriverFileDoesNotExistException(file);
    }
}

void AdgprsSimulator::UpdateFilePaths()
{
    script_args_ = (QStringList() << QString::fromStdString(paths_.GetPath(Paths::SIM_WORK_DIR ))
                                  << QString::fromStdString(paths_.GetPath(Paths::SIM_WORK_DIR ))
                                      + "/" + driver_file_name_);
}

bool AdgprsSimulator::Evaluate(int timeout, int threads) {
    script_args_[2] = QString::number(threads);
    int t = timeout;
    if (timeout < 10) t = 10; // Always let simulations run for at least 10 seconds
    if (results_->isAvailable()) results()->DumpResults();
    copyDriverFiles();
    driver_file_writer_->WriteDriverFile(QString::fromStdString(paths_.GetPath(Paths::SIM_WORK_DIR )));
    std::cout << "Starting monitored simulation with timeout " << timeout << std::endl;
    bool success = ::Utilities::Unix::ExecShellScriptTimeout(
        QString::fromStdString(paths_.GetPath(Paths::SIM_EXEC_SCRIPT_FILE)),
        script_args_, t);
    if (success) {
        paths_.SetPath(Paths::SIM_HDF5_FILE,
                       paths_.GetPath(Paths::SIM_WORK_DIR) + "/"
                           + driver_file_name_.split(".").first().toStdString() + ".vars.h5"
        );
        results_->ReadResults(output_h5_summary_file_path_);
    }
    updateResultsInModel();
    return success;
}
void AdgprsSimulator::WriteDriverFilesOnly() {
    copyDriverFiles();
    driver_file_writer_->WriteDriverFile(QString::fromStdString(paths_.GetPath(Paths::SIM_WORK_DIR )));
}
bool AdgprsSimulator::Evaluate(const Settings::Ensemble::Realization &realization, int timeout, int threads) {
    throw std::runtime_error("Ensemble optimization not yet implemented for the AD-GPRS reservoir simulator.");
}

}
}
