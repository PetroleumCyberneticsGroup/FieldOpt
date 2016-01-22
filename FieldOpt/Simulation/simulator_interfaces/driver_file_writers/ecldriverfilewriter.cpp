/******************************************************************************
 *
 *
 *
 * Created: 12.11.2015 2015 by einar
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

#include "ecldriverfilewriter.h"
#include "driver_parts/ecl_driver_parts/runspec_section.h"
#include "driver_parts/ecl_driver_parts/grid_section.h"
#include "driver_parts/ecl_driver_parts/props_section.h"
#include "driver_parts/ecl_driver_parts/solution_section.h"
#include "driver_parts/ecl_driver_parts/summary_section.h"
#include "driver_parts/ecl_driver_parts/schedule_section.h"
#include "Simulation/simulator_interfaces/simulator_exceptions.h"
#include "Utilities/file_handling/filehandling.h"

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {

EclDriverFileWriter::EclDriverFileWriter(Utilities::Settings::Settings *settings, Model::Model *model)
{
    model_ = model;
    settings_ = settings;
    original_driver_file_contents_ = ::Utilities::FileHandling::ReadFileToStringList(settings_->simulator()->driver_file_path());
    output_driver_file_name_ = settings->output_directory() + "/" + settings->name().toUpper() + ".DATA";
}

void EclDriverFileWriter::WriteDriverFile()
{
    DriverParts::ECLDriverParts::Runspec runspec = DriverParts::ECLDriverParts::Runspec(original_driver_file_contents_, model_->wells());
    DriverParts::ECLDriverParts::Grid grid = DriverParts::ECLDriverParts::Grid(original_driver_file_contents_);
    DriverParts::ECLDriverParts::Props props = DriverParts::ECLDriverParts::Props(original_driver_file_contents_);
    DriverParts::ECLDriverParts::Solution solution = DriverParts::ECLDriverParts::Solution(original_driver_file_contents_);
    DriverParts::ECLDriverParts::Summary summary = DriverParts::ECLDriverParts::Summary(original_driver_file_contents_);
    DriverParts::ECLDriverParts::Schedule schedule = DriverParts::ECLDriverParts::Schedule(model_->wells());

    QString complete_string = runspec.GetPartString() + grid.GetPartString()
            + props.GetPartString() + solution.GetPartString()
            + summary.GetPartString() + schedule.GetPartString();

    if (!Utilities::FileHandling::DirectoryExists(settings_->output_directory())
            || !Utilities::FileHandling::ParentDirectoryExists(output_driver_file_name_))
        throw UnableToWriteDriverFileException("Cannot write driver file, specified output directory does not exist.");

    Utilities::FileHandling::WriteStringToFile(complete_string, output_driver_file_name_);
}

}
}
}
