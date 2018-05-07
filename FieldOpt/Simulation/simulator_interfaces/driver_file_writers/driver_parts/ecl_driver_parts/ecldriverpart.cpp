/***********************************************************
 Created: 12.11.2015 2015 by einar

 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "ecldriverpart.h"

// ---------------------------------------------------------
namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {

// =========================================================
void ECLDriverPart::initializeBaseEntryLine(int n) {

  // -------------------------------------------------------
  base_entry_line_ = QStringList();
  while (base_entry_line_.size() < n) {
    base_entry_line_.append("1*");
  }
}

QStringList ECLDriverPart::GetBaseEntryLine(const int n) const {
    auto base_entry_line = QStringList();
    while (base_entry_line.size() < n) {
        base_entry_line.append("1*");
    }
    return base_entry_line;
}

}
}
}
}
}
