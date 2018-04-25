/***********************************************************
 Created: 18.11.2015 2015 by einar

 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version
 3 of the License, or (at your option) any later version.

 FieldOpt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the
 GNU General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef SCHEDULE_SECTION_H
#define SCHEDULE_SECTION_H

// ---------------------------------------------------------
#include "ecldriverpart.h"
#include "welspecs.h"
#include "compdat.h"
#include "wellcontrols.h"

#include "Model/wells/well.h"
#include "Model/wells/well_group.h"

// ---------------------------------------------------------
// QT / STD
#include <QStringList>

// ---------------------------------------------------------
namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {

// =========================================================
class Schedule : public ECLDriverPart
{
 public:
  // ---------------------------------------------------------
  Schedule(QList<Model::Wells::Well *> *wells,
           QList<double> control_times);

  QString GetPartString();

 private:
  // ---------------------------------------------------------
  Welspecs *welspecs_;
  Compdat *compdat_;
  WellControls *wellcontrols_;
};

}
}
}
}
}

#endif // SCHEDULE_SECTION_H
