/******************************************************************************
   Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef FIELDOPT_COMPSEGS_H
#define FIELDOPT_COMPSEGS_H

#include "Model/wells/segmented_well.h"
#include "ecldriverpart.h"

using namespace Model::Wells;

namespace Simulation {
namespace ECLDriverParts {

/*!
 * This class generates the COMPSEGS keyword. It must be called once pr. well.
 *
 * This keyword has two types of records.
 *
 * Record 1:
 *  1. Name of well.
 *
 * Remaining records:
 *  1. I-location of connection.
 *  2. J-location of connection.
 *  3. K-location of connection.
 *  4. Branch number.
 *  5. Distance down tubing from well's zero tubing length reference to _start_ of connection in this block.
 *  6. Distance down tubing from well's zero tubing length reference to _end_ of connection in this block.
 *  7. Direction of penetration.
 *  8. I-, J- or K-location of connection at end of range.
 *  9. Depth of center of connections in range.
 */
class Compsegs  : public ECLDriverPart {
 public:
  Compsegs(SegmentedWell *well);
  Compsegs() {}
  QString GetPartString() const override;

 private:
  QString generateEntry(SegmentedWell::Segment seg);
  QString wname_;
  QStringList entries_;
};

}
}

#endif //FIELDOPT_COMPSEGS_H
