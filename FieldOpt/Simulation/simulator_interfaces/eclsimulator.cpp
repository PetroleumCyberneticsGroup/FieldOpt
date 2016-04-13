/******************************************************************************
 *
 *
 *
 * Created: 16.10.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
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

#include "eclsimulator.h"
#include "Utilities/file_handling/filehandling.h"
#include "Utilities/unix/execution.h"
#include "simulator_exceptions.h"
#include "Simulation/results/eclresults.h"

namespace Simulation {
namespace SimulatorInterfaces {

ECLSimulator::ECLSimulator(Utilities::Settings::Settings *settings, Model::Model *model)
{
    if (settings->driver_path().length() == 0)
        throw DriverFileInvalidException("A path to a valid simulator driver file must be specified in the FieldOpt driver file or as a command line parameter.");

    if (Utilities::FileHandling::FileExists(settings->driver_path()))
        initial_driver_file_path_ = settings->driver_path();
    else throw DriverFileDoesNotExistException(settings->driver_path());

    if (Utilities::FileHandling::DirectoryExists(settings->output_directory()))
        output_directory_ = settings->output_directory();
    else throw OutputDirectoryDoesNotExistException(settings->output_directory());

    settings_ = settings;
    model_ = model;
    driver_file_writer_ = new DriverFileWriters::EclDriverFileWriter(settings, model_);

    script_path_ = ExecutionScripts::GetScriptPath(settings->simulator()->script_name());
    script_args_ = (QStringList() << output_directory_ << driver_file_writer_->output_driver_file_name_);

    results_ = new Results::ECLResults();
    try {
        results()->ReadResults(driver_file_writer_->output_driver_file_name_);
    } catch (...) {} // At this stage we don't really care if the results can be read, we just want to set the path.
}

void ECLSimulator::Evaluate()
{
    driver_file_writer_->WriteDriverFile();
    ::Utilities::Unix::ExecShellScript(script_path_, script_args_);
    results_->ReadResults(driver_file_writer_->output_driver_file_name_);
}

void ECLSimulator::CleanUp()
{
    QStringList file_endings_to_delete{"DBG", "ECLEND", "ECLRUN", "EGRID", "GRID",
                                       "h5", "INIT","INSPEC", "MSG", "PRT",
                                       "RSSPEC", "UNRST"};
    QString base_file_path = driver_file_writer_->output_driver_file_name_.split(".DATA").first();
    foreach (QString ending, file_endings_to_delete) {
        Utilities::FileHandling::DeleteFile(base_file_path + "." + ending);
    }
}

void ECLSimulator::UpdateFilePaths()
{
    return;
}

}
}
