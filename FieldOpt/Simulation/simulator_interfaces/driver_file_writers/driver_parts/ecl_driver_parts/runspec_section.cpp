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

#include "runspec_section.h"
#include "Simulation/simulator_interfaces/simulator_exceptions.h"

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {

Runspec::Runspec(QStringList *driver_file_contents, QList<Model::Wells::Well *> *wells)
{
    runspec_ = getSectionContent(driver_file_contents, "RUNSPEC", "GRID");

    // Remove original contents of WELLDIMS
    int start = runspec_.indexOf("WELLDIMS", 0, Qt::CaseInsensitive);
    int stop = runspec_.indexOf("/", start) + 1;
    runspec_.remove(start, stop-start);

    // Create proper WELLDIMS keyword
    int maxwells = wells->size();
    int maxconns = wells->first()->trajectory()->GetWellBlocks()->size() * (maxwells +1);
    int maxgroups = wells->size();
    QString welldims = QString("WELLDIMS\n   %1   %2   %3   %4 /\n").arg(maxwells).arg(maxconns).arg(maxgroups).arg(maxwells);

    // Insert proper welldims keyword
    runspec_.insert(start, welldims);
}

QString Runspec::GetPartString()
{
    return runspec_;
}

}
}
}
}
}
