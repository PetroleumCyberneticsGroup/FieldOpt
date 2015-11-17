/******************************************************************************
 *
 *
 *
 * Created: 17.11.2015 2015 by einar
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

#include "grid_section.h"
#include "Simulation/simulator_interfaces/simulator_exceptions.h"

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {

Grid::Grid(QStringList *driver_file_contents)
{
    // Find start
    int start_index = 0;
    while (true) {
        if (driver_file_contents->at(start_index).startsWith("GRID"))
            break; // Found the start of the GRID section
        else {
            start_index++;
            if (start_index >= driver_file_contents->size())
                throw DriverFileInvalidException("Did not find the GRID section in the driver file.");
        }
    }

    // Add grid content to the runspec_ string.
    grid_ = "";
    for (int line = start_index; line < driver_file_contents->size(); ++line) {
        if (driver_file_contents->at(line).startsWith("EDIT") || driver_file_contents->at(line).startsWith("PROPS")) // If we're at the next section, break
            break;
        else {
            grid_.append(driver_file_contents->at(line) + "\n");
        }
    }
}

QString Grid::GetPartString()
{
    return grid_;
}

}
}
}
}
}
