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

#ifndef FIELDOPT_WELSEGS_H
#define FIELDOPT_WELSEGS_H

#include <Model/wells/well.h>
#include "ecldriverpart.h"

using namespace Model::Wells;

namespace Simulation {
namespace ECLDriverParts {

/*!
 * Generates the WELSEGS keyword. One instance of this must be created pr. well.
 *
 * The WELSEGS keyword consists of two different types of records:
 * The first record identifies the well, and sets the heel segment node;
 * The remaining nodes specify the segments.
 *
 * Record 1:
 *   1. Name of well
 *   2. TVD of top segment.
 *   3. MD to first segment.
 *   4. Effective wellbore volume of top segment.
 *   5. Type of tubing and depth information: INC(remental) or ABS(olute).
 *   6. Components of pressure drop: 'HFA', 'HF-' or 'H--'.
 *   7. Flow model: HO or DF.
 *   8. X-coordinate of nodal point.
 *   9. Y-coordinate of nodal point.
 *
 * Subsequent records:
 *   1. Segment number (first in range).
 *   2. Segment number (last in range).
 *   3. Branch number.
 *   4. Outlet segment number.
 *   5. Length of segment.
 *   6. Depth change through segment.
 *   7. Diameter.
 *   8. Roughness.
 *   9. Cross sectional area for fluid flow.
 *  10. Volume of segment.
 *  11. Length of segment projected onto X-axis.
 *  12. Length of segment projected onto Y-axis.
 */
class Welsegs : public ECLDriverPart {
 public:
  Welsegs() {};

  Welsegs(Well *well);

  /*!
   * Loop through wells. For all segmented wells, check if start time eqials ts. If yes, generate entries.
   * @param wells Wells to loop through
   * @param ts Timestep to write entries for.
   */
  Welsegs(QList<Model::Wells::Well *> *wells, int ts);

  QString GetPartString() const override;

 private:
  /*!
   * Holds strings for _one_ welsegs keyword. This is necessary
   * because one needs one keyword pr. well, unlike most other
   * keywords where you can specify multiple wells in one keyword.
   */
  struct WelsegsKeyword {
    QString heel_entry;
    QStringList seg_entries;
    QString buildKeyword() const;
  };
  QString createHeelEntry(Well *well);
  QString createSegmentEntry(Segment segment);

  QList<WelsegsKeyword> keywords_;
};

}
}
#endif //FIELDOPT_WELSEGS_H
