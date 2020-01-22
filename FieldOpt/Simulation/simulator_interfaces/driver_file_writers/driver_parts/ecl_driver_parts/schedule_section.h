/******************************************************************************
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2019 Einar J.M. Baumann <einar.baumann@ntnu.no>
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

#ifndef SCHEDULE_SECTION_H
#define SCHEDULE_SECTION_H

#include "ecldriverpart.h"
#include "Model/wells/well.h"
#include "welspecs.h"
#include "compdat.h"
#include "wellcontrols.h"
#include "welsegs.h"
#include "compsegs.h"
#include "wsegvalv.h"
#include "schedule_insets.h"
#include <QStringList>

namespace Simulation {
namespace ECLDriverParts {

class Schedule : public ECLDriverPart
{
 public:
  /*!
   * @brief Constructor. Generate the time entries and build the complete schedule string.
   * @param wells List of wells from the model.
   * @param control_times List of control times.
   * @param insets Text snippets to be inserted at specific time steps in the schedule.
   */
  Schedule(QList<Model::Wells::Well *> *wells, QList<int> control_times, ScheduleInsets &insets);
  QString GetPartString() const;

  struct ScheduleTimeEntry {
    ScheduleTimeEntry(int control_time,
                      Welspecs welspecs,
                      Compdat compdat,
                      WellControls well_controls,
                      Welsegs welsegs,
                      Compsegs compsegs,
                      Wsegvalv wsegvalv
    );
    int control_time;

    Welspecs welspecs;
    Compdat compdat;
    WellControls well_controls;

    // Exclusively for segmented wells
    Welsegs welsegs;
    Compsegs compsegs;
    Wsegvalv wsegvalv;
  };

 private:
  /*!
   * WELSPECS, COMPDAT and control entries for each timestep.
   */
  QList<ScheduleTimeEntry> schedule_time_entries_;

  QString schedule_;

 public:
  QList<ScheduleTimeEntry> GetScheduleTimeEntries() { return schedule_time_entries_; }
};

}
}

#endif // SCHEDULE_SECTION_H
