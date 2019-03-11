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

#include <Utilities/printer.hpp>
#include "ecldriverfilewriter.h"
#include "driver_parts/ecl_driver_parts/schedule_section.h"
#include "Simulation/simulator_interfaces/simulator_exceptions.h"
#include "Utilities/filehandling.hpp"
#include "Utilities/verbosity.h"

namespace Simulation {

using namespace ECLDriverParts;
using namespace Utilities::FileHandling;

EclDriverFileWriter::EclDriverFileWriter(Settings::Settings *settings, Model::Model *model)
{
    model_ = model;
    settings_ = settings;

    if (settings->paths().IsSet(Paths::SIM_SCH_INSET_FILE)) {
        insets_ = ECLDriverParts::ScheduleInsets(settings->paths().GetPath(Paths::SIM_SCH_INSET_FILE));
    }
}

void EclDriverFileWriter::WriteDriverFile(QString schedule_file_path)
{
    if (VERB_SIM >= 2) {
        auto fp = schedule_file_path.toStdString();
        Printer::ext_info("Writing driver file to " + fp + ".", "Simulation", "EclDriverFileWriter");
    }
    assert(FileExists(schedule_file_path));
    Schedule schedule = ECLDriverParts::Schedule(model_->wells(), settings_->model()->control_times(), insets_);
    model_->SetCompdatString(ECLDriverParts::Compdat(model_->wells()).GetPartString());
    Utilities::FileHandling::WriteStringToFile(schedule.GetPartString(), schedule_file_path);
}

}
