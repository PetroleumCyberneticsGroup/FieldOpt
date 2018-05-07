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
#include "Utilities/filehandling.hpp"

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {

using namespace DriverParts::ECLDriverParts;
using namespace Utilities::FileHandling;

EclDriverFileWriter::EclDriverFileWriter(Settings::Settings *settings, Model::Model *model)
{
    model_ = model;
    settings_ = settings;
}

void EclDriverFileWriter::WriteDriverFile(QString schedule_file_path)
{
    if (settings_->verbose()) {
        std::cout << "Writing driver file to " << schedule_file_path.toStdString() << std::endl;
    }
    assert(FileExists(schedule_file_path));
    Schedule schedule = DriverParts::ECLDriverParts::Schedule(model_->wells(), settings_->model()->control_times());
    model_->SetCompdatString(DriverParts::ECLDriverParts::Compdat(model_->wells()).GetPartString());
    Utilities::FileHandling::WriteStringToFile(schedule.GetPartString(), schedule_file_path);
}

}
}
}
