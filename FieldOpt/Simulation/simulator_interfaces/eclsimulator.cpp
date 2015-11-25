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
#include "driver_file_writers/driver_parts/ecl_driver_parts/runspec_section.h"
#include "driver_file_writers/driver_parts/ecl_driver_parts/grid_section.h"
#include "driver_file_writers/driver_parts/ecl_driver_parts/props_section.h"
#include "driver_file_writers/driver_parts/ecl_driver_parts/solution_section.h"
#include "driver_file_writers/driver_parts/ecl_driver_parts/summary_section.h"
#include "driver_file_writers/driver_parts/ecl_driver_parts/schedule_section.h"
#include "Utilities/file_handling/filehandling.h"
#include "Utilities/unix/execution.h"
#include "simulator_exceptions.h"
#include "Simulation/execution_scripts/execution_scripts.h"

namespace Simulation {
namespace SimulatorInterfaces {

namespace DriverFileParts = DriverFileWriters::DriverParts::ECLDriverParts;

ECLSimulator::ECLSimulator(Utilities::Settings::Settings *settings, Model::Model *model)
{
    if (Utilities::FileHandling::FileExists(settings->driver_path()))
        initial_driver_file_path_ = settings->driver_path();
    else throw DriverFileDoesNotExistException(settings->driver_path());

    if (Utilities::FileHandling::DirectoryExists(settings->output_directory()))
        output_directory_ = settings->output_directory();
    else throw OutputDirectoryDoesNotExistException(settings->output_directory());

    settings_ = settings;
    model_ = model;
    original_driver_file_contents_ = ::Utilities::FileHandling::ReadFileToStringList(settings_->simulator()->driver_file_path());
    output_driver_file_name_ = output_directory_ + "/" + settings->name().toUpper() + ".DATA";
}

void ECLSimulator::Evaluate()
{
    writeDriverFile();
    QStringList args {output_directory_, output_driver_file_name_};
    QString script = Simulation::ExecutionScripts::DefaultScripts[Simulation::ExecutionScripts::Script::csh_eclrun];
    ::Utilities::Unix::ExecShellScript(script, args);
}

void ECLSimulator::CleanUp()
{
    QStringList file_endings_to_delete{"DBG", "ECLEND", "ECLRUN", "EGRID", "GRID",
                                       "h5", "INIT","INSPEC", "MSG", "PRT",
                                       "RSSPEC", "UNRST"};
    QString base_file_path = output_driver_file_name_.split(".DATA").first();
    foreach (QString ending, file_endings_to_delete) {
        Utilities::FileHandling::DeleteFile(base_file_path + "." + ending);
    }
}

void ECLSimulator::writeDriverFile()
{
    DriverFileParts::Runspec runspec = DriverFileParts::Runspec(original_driver_file_contents_);
    DriverFileParts::Grid grid = DriverFileParts::Grid(original_driver_file_contents_);
    DriverFileParts::Props props = DriverFileParts::Props(original_driver_file_contents_);
    DriverFileParts::Solution solution = DriverFileParts::Solution(original_driver_file_contents_);
    DriverFileParts::Summary summary = DriverFileParts::Summary(original_driver_file_contents_);
    DriverFileParts::Schedule schedule = DriverFileParts::Schedule(model_->wells());

    QString complete_string = runspec.GetPartString() + grid.GetPartString()
            + props.GetPartString() + solution.GetPartString()
            + summary.GetPartString() + schedule.GetPartString();

    if (!Utilities::FileHandling::DirectoryExists(output_directory_) || !Utilities::FileHandling::ParentDirectoryExists(output_driver_file_name_))
        throw UnableToWriteDriverFileException("Cannot write driver file, specified output directory does not exist.");

    Utilities::FileHandling::WriteStringToFile(complete_string, output_driver_file_name_);
}

}
}
