/******************************************************************************
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#include <Utilities/printer.hpp>
#include <simulator_interfaces/driver_file_writers/ix_driver_file_writer.h>
#include <Utilities/verbosity.h>
#include <Utilities/execution.hpp>
#include "Simulation/results/eclresults.h"
#include "ix_simulator.h"

namespace Simulation {

IXSimulator::IXSimulator(Settings::Settings *settings, Model::Model *model)
    : Simulator(settings)
{
    model_ = model;
    settings_ = settings;
    if (paths_.IsSet(Paths::ENSEMBLE_FILE)) {
        throw std::runtime_error("Multiple realization is not yet supported in the INTERSECT interface.");
    }
    deck_name_ = driver_file_name_.split(".afi").first();
    results_ = new Results::ECLResults();
    result_path_ = "";
}
void IXSimulator::Evaluate() {
    copyDriverFiles();
    UpdateFilePaths();
    script_args_ = (QStringList() << QString::fromStdString(paths_.GetPath(Paths::SIM_WORK_DIR)) << deck_name_);
    auto driver_file_writer = IXDriverFileWriter(model_);
    driver_file_writer.WriteDriverFile(paths_.GetPath(Paths::SIM_OUT_SCH_FILE));
    if (VERB_SIM >= 1) { Printer::ext_info("Starting unmonitored evaluation.", "Simulation", "IXSimulator"); }
    Utilities::Unix::ExecShellScript(
        QString::fromStdString(paths_.GetPath(Paths::SIM_EXEC_SCRIPT_FILE)),
        script_args_
    );
    results_->DumpResults();
    if (result_path_.size() == 0) {
        setResultPath();
    }
    if (VERB_SIM >= 1) { Printer::info("Unmonitored simulation done. Reading results from " + result_path_.toStdString()); }
    results_->ReadResults(result_path_);
    updateResultsInModel();
}
bool IXSimulator::Evaluate(int timeout, int threads) {
    copyDriverFiles();
    UpdateFilePaths();
    script_args_ = (QStringList() << QString::fromStdString(paths_.GetPath(Paths::SIM_WORK_DIR)) << deck_name_ << QString::number(threads));
    auto driver_file_writer = IXDriverFileWriter(model_);
    driver_file_writer.WriteDriverFile(paths_.GetPath(Paths::SIM_OUT_SCH_FILE));
    int t = timeout;
    if (timeout < 10) {
        t = 10; // Always let simulations run for at least 10 seconds
    }

    if (VERB_SIM >= 1) { Printer::info("Starting monitored simulation with timeout."); }
    bool success = ::Utilities::Unix::ExecShellScriptTimeout(
        QString::fromStdString(paths_.GetPath(Paths::SIM_EXEC_SCRIPT_FILE)),
        script_args_, t);
    if (success) {
        results_->DumpResults();
        if (result_path_.size() == 0) {
            setResultPath();
        }
        if (VERB_SIM >= 1) { Printer::info("Simulation successful. Reading results from " + result_path_.toStdString()); }
        results_->ReadResults(result_path_);
    }
    else {
        if (VERB_SIM >= 1) { Printer::info("Simulation failed."); }
    }
    updateResultsInModel();
    return success;
}
bool IXSimulator::Evaluate(const Settings::Ensemble::Realization &realization, int timeout, int threads) {
    throw std::runtime_error("Multiple realization is not yet supported in the INTERSECT interface.");
}
void IXSimulator::WriteDriverFilesOnly() {
    UpdateFilePaths();
    auto driver_file_writer = IXDriverFileWriter(model_);
    driver_file_writer.WriteDriverFile(paths_.GetPath(Paths::SIM_OUT_SCH_FILE));
}
void IXSimulator::CleanUp() {
    Printer::ext_warn("CleanUp not yet implemented for IX interface.", "Simulation", "IXSimulator");
}
void IXSimulator::UpdateFilePaths() {
    paths_.SetPath(Paths::SIM_WORK_DIR,        paths_.GetPath(Paths::OUTPUT_DIR)   + "/" + driver_parent_dir_name_.toStdString());
    paths_.SetPath(Paths::SIM_OUT_DRIVER_FILE, paths_.GetPath(Paths::SIM_WORK_DIR) + "/" + driver_file_name_.toStdString());

    std::string tmp = paths_.GetPath(Paths::SIM_SCH_FILE);
    boost::algorithm::replace_first(tmp, paths_.GetPath(Paths::SIM_DRIVER_DIR), paths_.GetPath(Paths::SIM_WORK_DIR));
    paths_.SetPath(Paths::SIM_OUT_SCH_FILE, tmp);
}

void IXSimulator::copyDriverFiles() {
    std::string workdir = paths_.GetPath(Paths::OUTPUT_DIR) + "/" + driver_parent_dir_name_.toStdString();

    if (!DirectoryExists(workdir)) {
        if (VERB_SIM >= 1) {
            Printer::ext_info("Output deck directory not found. Copying input deck:"
                                  + paths_.GetPath(Paths::SIM_DRIVER_DIR) + " -> " + workdir,
                                  "Simulation", "IXSimulator" );
        }
        CreateDirectory(workdir);
        CopyDirectory(paths_.GetPath(Paths::SIM_DRIVER_DIR), workdir, false);
        if (paths_.IsSet(Paths::SIM_AUX_DIR)) {
            std::string auxdir = paths_.GetPath(Paths::OUTPUT_DIR) + "/" + FileName(paths_.GetPath(Paths::SIM_AUX_DIR));
            if (!DirectoryExists(auxdir)) {
                if (VERB_SIM >= 1) {
                    Printer::ext_info("Copying simulation aux. directory:"
                                          + paths_.GetPath(Paths::SIM_AUX_DIR) + " -> " + auxdir,
                                          "Simulation", "IXSimulator" );
                }
                CreateDirectory(auxdir);
                CopyDirectory(paths_.GetPath(Paths::SIM_AUX_DIR), auxdir, false);
            }
        }
    }
    paths_.SetPath(Paths::SIM_WORK_DIR, workdir);
    if (VERB_SIM >= 2) {
        Printer::ext_info("Done copying directories. Set working directory to: " + workdir,
            "Simulation", "IXSimulator");
    }
}
void IXSimulator::setResultPath() {
    QString result_dir = QString::fromStdString(paths_.GetPath(Paths::SIM_WORK_DIR));
    QString result_name;
    if (VERB_SIM >= 2) Printer::ext_info("Setting simulator result path.", "Simulation", "IXSimulator");
    if (FileExists(result_dir + "/SUMMARYVECS.SMSPEC")) {
        result_name = "/SUMMARYVECS.SMSPEC";
    }
    else if (FileExists(result_dir + "/" + deck_name_ + "_SUMMARYVECS.SMSPEC")) {
        result_name = "/" + deck_name_ + "_SUMMARYVECS.SMSPEC";
    }
    else {
        Printer::error("Unable to find summary file. Aborting.");
        exit(1);
    }
    result_path_ = result_dir + result_name;
    if (VERB_SIM >= 2) Printer::ext_info("Set simulator result path to " + result_path_.toStdString(), "Simulation", "IXSimulator");
}

}
