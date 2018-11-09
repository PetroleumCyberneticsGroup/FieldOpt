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
#include <boost/algorithm/string.hpp>
#include <Utilities/printer.hpp>
#include <Utilities/verbosity.h>
#include "eclsimulator.h"
#include "Utilities/execution.hpp"
#include "simulator_exceptions.h"
#include "Simulation/results/eclresults.h"

namespace Simulation {

using namespace Utilities::FileHandling;

ECLSimulator::ECLSimulator(Settings::Settings *settings, Model::Model *model)
    : Simulator(settings)
{
    model_ = model;
    settings_ = settings;
    if (!paths_.IsSet(Paths::ENSEMBLE_FILE)) {
        deck_name_ = driver_file_name_.split(".DATA").first();
    }
    else {
        deck_name_ = "";
    }

    results_ = new Results::ECLResults();
}

void ECLSimulator::Evaluate()
{
    if (VERB_SIM >= 2) { Printer::ext_info("Starting unmonitored evaluation.", "Simulation", "ECLSimulator"); }
    if (VERB_SIM >= 2) { Printer::info("Copying driver files."); }
    copyDriverFiles();
    if (VERB_SIM >= 2) { Printer::info("Updating file paths."); }
    UpdateFilePaths();
    script_args_ = (QStringList() << QString::fromStdString(paths_.GetPath(Paths::SIM_WORK_DIR)) << deck_name_);
    auto driver_file_writer = EclDriverFileWriter(settings_, model_);
    if (VERB_SIM >= 2) { Printer::info("Writing schedule."); }
    driver_file_writer.WriteDriverFile(QString::fromStdString(paths_.GetPath(Paths::SIM_OUT_SCH_FILE)));
    if (VERB_SIM >= 2) { Printer::info("Starting unmonitored simulation."); }
    Utilities::Unix::ExecShellScript(
        QString::fromStdString(paths_.GetPath(Paths::SIM_EXEC_SCRIPT_FILE)),
        script_args_
    );
    if (VERB_SIM >= 2) { Printer::info("Unmonitored simulation done. Reading results."); }
    results_->ReadResults(QString::fromStdString(paths_.GetPath(Paths::SIM_OUT_DRIVER_FILE)));
    updateResultsInModel();
}

bool ECLSimulator::Evaluate(int timeout, int threads) {
    copyDriverFiles();
    UpdateFilePaths();
    script_args_ = (QStringList() << QString::fromStdString(paths_.GetPath(Paths::SIM_WORK_DIR)) << deck_name_ << QString::number(threads));
    auto driver_file_writer = EclDriverFileWriter(settings_, model_);
    driver_file_writer.WriteDriverFile(QString::fromStdString(paths_.GetPath(Paths::SIM_OUT_SCH_FILE)));
    int t = timeout;
    if (timeout < 10) {
        t = 10; // Always let simulations run for at least 10 seconds
    }

    if (VERB_SIM >= 2) {
        Printer::info("Starting monitored simulation with timeout.");
    }
    bool success = ::Utilities::Unix::ExecShellScriptTimeout(
        QString::fromStdString(paths_.GetPath(Paths::SIM_EXEC_SCRIPT_FILE)),
        script_args_, t);
    if (VERB_SIM >= 2) {
        Printer::info("Monitored simulation done.");
    }
    if (success) {
        if (VERB_SIM >= 2) {
            Printer::info("Simulation successful. Reading results.");
        }
        results_->DumpResults();
        results_->ReadResults(QString::fromStdString(paths_.GetPath(Paths::SIM_OUT_DRIVER_FILE)));
    }
    updateResultsInModel();
    return success;
}

bool ECLSimulator::Evaluate(const Settings::Ensemble::Realization &realization, int timeout, int threads) {
    driver_file_name_ = QString::fromStdString(FileName(realization.data()));
    driver_parent_dir_name_ = QString::fromStdString(ParentDirectoryName(realization.data()));
    deck_name_ = driver_file_name_.split(".").first();
    paths_.SetPath(Paths::SIM_DRIVER_FILE, realization.data());
    paths_.SetPath(Paths::SIM_DRIVER_DIR , GetParentDirectoryPath(realization.data()));
    paths_.SetPath(Paths::SIM_SCH_FILE   , realization.schedule());
    return Evaluate(timeout, threads);
}

void ECLSimulator::CleanUp()
{
    UpdateFilePaths();
    QStringList file_endings_to_delete{"DBG", "ECLEND", "ECLRUN", "EGRID", "GRID",
                                       "h5", "INIT","INSPEC", "MSG", "PRT",
                                       "RSSPEC", "UNRST"};
    QString base_file_path = QString::fromStdString(paths_.GetPath(Paths::SIM_OUT_DRIVER_FILE)).split(".DATA").first();

    for (QString ending : file_endings_to_delete) {
        DeleteFile(base_file_path + "." + ending);
    }
}

void ECLSimulator::UpdateFilePaths()
{
    paths_.SetPath(Paths::SIM_WORK_DIR,        paths_.GetPath(Paths::OUTPUT_DIR) + "/" + driver_parent_dir_name_.toStdString());
    paths_.SetPath(Paths::SIM_OUT_DRIVER_FILE, paths_.GetPath(Paths::SIM_WORK_DIR) + "/" + driver_file_name_.toStdString());

    std::string tmp = paths_.GetPath(Paths::SIM_SCH_FILE);
    boost::algorithm::replace_first(tmp, paths_.GetPath(Paths::SIM_DRIVER_DIR), paths_.GetPath(Paths::SIM_WORK_DIR));
    paths_.SetPath(Paths::SIM_OUT_SCH_FILE, tmp);
}

void ECLSimulator::WriteDriverFilesOnly() {
    UpdateFilePaths();
    auto driver_file_writer = EclDriverFileWriter(settings_, model_);
    driver_file_writer.WriteDriverFile(QString::fromStdString(paths_.GetPath(Paths::SIM_OUT_SCH_FILE)));
}

void ECLSimulator::copyDriverFiles() {
    std::string workdir = paths_.GetPath(Paths::OUTPUT_DIR) + "/" + driver_parent_dir_name_.toStdString();

    if (!DirectoryExists(workdir)) {
        if (VERB_SIM >= 1) {
            Printer::ext_info("Output deck directory not found. Copying input deck:"
            + paths_.GetPath(Paths::SIM_DRIVER_DIR) + " -> " + workdir, "Simulation", "ECLSimulator" );
        }
        CreateDirectory(workdir);
        CopyDirectory(paths_.GetPath(Paths::SIM_DRIVER_DIR), workdir, false);
    }
    if (paths_.IsSet(Paths::SIM_AUX_DIR)) {
        std::string auxdir = paths_.GetPath(Paths::OUTPUT_DIR) + "/" + FileName(paths_.GetPath(Paths::SIM_AUX_DIR));
        if (!DirectoryExists(auxdir)) {
            if (VERB_SIM >= 1) {
                Printer::ext_info("Copying simulation aux. directory:"
                                      + paths_.GetPath(Paths::SIM_AUX_DIR) + " -> " + auxdir, "Simulation", "Simulator" );
            }
            CreateDirectory(auxdir);
            CopyDirectory(paths_.GetPath(Paths::SIM_AUX_DIR), auxdir, false);
        }
    }
    paths_.SetPath(Paths::SIM_WORK_DIR, workdir);
    if (VERB_SIM >= 2) {
        Printer::ext_info("Done copying directories. Set working directory to: " + workdir,
                          "Simulation",
                          "ECLSimulator");
    }
}

}
