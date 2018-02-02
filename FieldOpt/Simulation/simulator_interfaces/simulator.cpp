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

Simulator::Simulator(Settings::Settings *settings) {
    settings_ = settings;

    if (settings->driver_path().length() == 0)
        throw DriverFileInvalidException(
            "A path to a valid simulator driver file must be specified "
                "in the FieldOpt driver file or as a command line parameter.");

    if (!Utilities::FileHandling::FileExists(settings->simulator()->driver_file_path()))
        DriverFileDoesNotExistException(settings->simulator()->driver_file_path());

    initial_driver_file_path_ = settings->simulator()->driver_file_path();
    control_times_ = settings->model()->control_times();

    QStringList tmp = initial_driver_file_path_.split("/");
    initial_driver_file_name_ = tmp.last();

    if (!Utilities::FileHandling::DirectoryExists(settings->output_directory()))
        OutputDirectoryDoesNotExistException(settings->output_directory());
    output_directory_ = settings->output_directory();

    if (settings->build_path().length() > 0)
        build_dir_ = settings->build_path() + "/";

    if (settings->simulator()->custom_simulator_execution_script_path().length() > 0)
        script_path_ = settings->simulator()->custom_simulator_execution_script_path();
    else
        script_path_ = build_dir_ + ExecutionScripts::GetScriptPath(settings->simulator()->script_name());
    script_args_ = (QStringList() << output_directory_
                                  << output_directory_+"/"+initial_driver_file_name_
                                  << QString::number(1));

    if (settings_->verbosity_level() > 4) {
        std::cout << "Simulator. -> " << std::endl
                  << "initial_driver_file_path_: " << initial_driver_file_path_.toStdString()
                  << "initial_driver_file_name_: " << initial_driver_file_name_.toStdString()
                  << "control_times_.size() : " << control_times_.size()
                  << "script_path_: " << script_path_.toStdString()
                  << "output_directory_: " << script_args_.at(0).toStdString()
                  << "script_args_[0]: " << script_args_.at(0).toStdString()
                  << "script_args_[1]: " << script_args_.at(1).toStdString()
                  << std::endl;
    }
}

void Simulator::SetOutputDirectory(QString output_directory)
{
    if (Utilities::FileHandling::DirectoryExists(output_directory)) {
        output_directory_ = output_directory;
        UpdateFilePaths();
    }
    else throw OutputDirectoryDoesNotExistException(output_directory);
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
