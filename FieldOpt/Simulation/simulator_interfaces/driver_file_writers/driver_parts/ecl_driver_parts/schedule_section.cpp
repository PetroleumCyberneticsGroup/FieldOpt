/******************************************************************************
 *
 *
 *
 * Created: 18.11.2015 2015 by einar
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

#include "schedule_section.h"


namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {

Schedule::Schedule(QList<Model::Wells::Well *> *wells)
{
    welspecs_ = new Welspecs(wells);
    compdat_ = new Compdat(wells);
    wellcontrols_ = new WellControls(wells);
}

QString Schedule::GetPartString()
{
    return QString("SCHEDULE\n\n%1%2%3\n\nEND")
            .arg(welspecs_->GetPartString())
            .arg(compdat_->GetPartString())
            .arg(wellcontrols_->GetPartString());
}

}
}
}
}
}
