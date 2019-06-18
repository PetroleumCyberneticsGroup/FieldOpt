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
#include "simulator.h"
#include "simulator_exceptions.h"
#include "Utilities/execution.hpp"
#include "Simulation/results/json_results.h"

namespace Simulation {

using namespace Utilities::FileHandling;

Simulator::Simulator(Settings::Settings *settings) {
    settings_ = settings;
    paths_ = settings_->paths();

    if (!paths_.IsSet(Paths::ENSEMBLE_FILE)) { // single realization
        driver_file_name_ = QString::fromStdString(FileName(paths_.GetPath(Paths::SIM_DRIVER_FILE)));
        driver_parent_dir_name_ = QString::fromStdString(ParentDirectoryName(paths_.GetPath(Paths::SIM_DRIVER_FILE)));
    }
    else { // multiple realizations
        driver_file_name_ = "";
        driver_parent_dir_name_ = "";
    }

    // Use custom execution script if provided in runtime settings, else use the one from json driver file
    if (!paths_.IsSet(Paths::SIM_EXEC_SCRIPT_FILE)) {
        std::string exec_script_path = paths_.GetPath(Paths::BUILD_DIR)
                                       + ExecutionScripts::GetScriptPath(settings->simulator()->script_name()).toStdString();
        paths_.SetPath(Paths::SIM_EXEC_SCRIPT_FILE, exec_script_path);
    }
    script_args_ = (QStringList() << QString::fromStdString(paths_.GetPath(Paths::OUTPUT_DIR))
                                  << QString::fromStdString(paths_.GetPath(Paths::OUTPUT_DIR)) + "/" + driver_file_name_
                                  << QString::number(1));

    control_times_ = settings->model()->control_times();
}

::Simulation::Results::Results *Simulator::results()
{
    return results_;
}

void Simulator::PostSimWork() {
    if (settings_->simulator()->use_post_sim_script()) {
        std::string expected_scr_path = paths_.GetPath(Paths::SIM_WORK_DIR) + "/FO_POSTSIM.sh";
        if (VERB_SIM >= 2) Printer::ext_info("Looking for PostSimWork script at " + expected_scr_path, "Simulation", "Simulator");
        if (Utilities::FileHandling::FileExists(expected_scr_path)) {
            if (VERB_SIM >= 2) Printer::ext_info("PostSimWork script found. Executing... ", "Simulation", "Simulator");
            Utilities::Unix::ExecShellScript(QString::fromStdString(expected_scr_path), QStringList());
            if (VERB_SIM >= 2) Printer::ext_info("Done executing PostSimWork script.", "Simulation", "Simulator");
        }
        else {
            Printer::ext_warn("PostSimWork script not found.");
        }
    }
    if (settings_->simulator()->read_external_json_results()) {
        std::string expected_res_path = paths_.GetPath(Paths::SIM_WORK_DIR) + "/FO_EXT_RESULTS.json";
        if (VERB_SIM >= 2) Printer::ext_info("Reading external JSON results at " + expected_res_path, "Simulation", "Simulator");
        auto json_results = Simulation::Results::JsonResults(expected_res_path);
        results_->SetJsonResults(json_results);
    }
}

void Simulator::SetVerbosityLevel(int level) {
    verbosity_level_ = level;
}

void Simulator::updateResultsInModel() {
    model_->SetResult("Time", results_->GetValueVector(Results::Results::Property::Time));
    model_->SetResult("FGPT", results_->GetValueVector(Results::Results::Property::CumulativeGasProduction));
    model_->SetResult("FOPT", results_->GetValueVector(Results::Results::Property::CumulativeOilProduction));
    model_->SetResult("FWPT", results_->GetValueVector(Results::Results::Property::CumulativeWaterProduction));
}

}
