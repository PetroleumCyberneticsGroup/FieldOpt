/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 Created: 18.11.2015 2015 by einar

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include "schedule_section.h"

// ---------------------------------------------------------
namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {

Schedule::Schedule(QList<Model::Wells::Well *> *wells,
                   QList<double> control_times)
{
  schedule_time_entries_ = QList<ScheduleTimeEntry>();
  for (double ts : control_times) {
    ScheduleTimeEntry time_entry = ScheduleTimeEntry(Welspecs(wells, ts),
                                                     Compdat(wells, ts),
                                                     WellControls(wells, control_times, ts));
    schedule_time_entries_.append(time_entry);
  }
  for (auto time_entry : schedule_time_entries_) {
    schedule_.append(time_entry.welspecs.GetPartString());
    schedule_.append(time_entry.compdat.GetPartString());
    schedule_.append(time_entry.well_controls.GetPartString());
  }
  schedule_.append("\n\n");
}

QString Schedule::GetPartString() const
{
  return schedule_;
}

Schedule::ScheduleTimeEntry::ScheduleTimeEntry(ECLDriverParts::Welspecs welspecs,
                                               ECLDriverParts::Compdat compdat,
                                               ECLDriverParts::WellControls well_controls)
{
  this->welspecs = welspecs;
  this->compdat = compdat;
  this->well_controls = well_controls;
}

}
}
}
}
}
