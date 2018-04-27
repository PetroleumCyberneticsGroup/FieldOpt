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

namespace Simulation {
namespace SimulatorInterfaces {

using namespace Utilities::FileHandling;

Simulator::Simulator(Settings::Settings *settings) {
    settings_ = settings;
    paths_ = settings_->paths();

    driver_file_name_ =  QString::fromStdString(FileName(paths_.GetPath(Paths::SIM_DRIVER_FILE)));
    driver_parent_dir_name_ = QString::fromStdString(ParentDirectoryName(paths_.GetPath(Paths::SIM_DRIVER_FILE)));

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

void Simulator::SetVerbosityLevel(int level) {
    verbosity_level_ = level;
}

void Simulator::updateResultsInModel() {
    model_->SetResult("Time", results_->GetValueVector(Results::Results::Property::Time));
    model_->SetResult("Time", results_->GetValueVector(Results::Results::Property::CumulativeGasProduction));
    model_->SetResult("Time", results_->GetValueVector(Results::Results::Property::CumulativeOilProduction));
    model_->SetResult("Time", results_->GetValueVector(Results::Results::Property::CumulativeWaterProduction));
}

}
}
