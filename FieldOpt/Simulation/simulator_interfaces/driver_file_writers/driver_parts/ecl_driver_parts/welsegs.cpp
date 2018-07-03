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

#include "welsegs.h"

namespace Simulation {
namespace ECLDriverParts {
Welsegs::Welsegs(SegmentedWell *well) {
    head_ = "WELSEGS\n";
    foot_ = "/\n\n";
    heel_entry_ = createHeelEntry(well);
    auto segs = well->GetSegments();
    for (int i = 1; i < segs.size(); ++i) {
        seg_entries_.push_back(createSegmentEntry(segs[i]));
    }
}
Welsegs::Welsegs() { }
QString Welsegs::GetPartString() const {
    QString keyword = head_;
    keyword.append(heel_entry_ + "\n");
    keyword.append(seg_entries_.join("\n") + "\n");
    keyword.append(foot_);
    return keyword;
}
QString Welsegs::createHeelEntry(SegmentedWell *well) {
 /*
  * 0.   Name of well
  * 1.   TVD of top segment.
  * 2. * MD to first segment.
  * 3.   Effective wellbore volume of top segment.
  * 4.   Type of tubing and depth information: INC(remental) or ABS(olute).
  * 5.   Components of pressure drop: 'HFA', 'HF-' or 'H--'.
  * 6.   Flow model: HO or DF.
  * 7.   X-coordinate of nodal point.
  * 8.   Y-coordinate of nodal point.
  */
    auto rseg = well->GetSegments()[0];
    auto record = GetBaseEntryLine(10);
    record[0] = well->name();
    record[1] = QString::number(rseg.TVD());
    record[4] = "INC";
    record[5] = "HF-";
    record[6] = "DF";
    record[7] = well->trajectory()->GetWellBlocks()->at(0)->getEntryPoint().x();
    record[8] = well->trajectory()->GetWellBlocks()->at(0)->getEntryPoint().y();
    return record.join("  ");
}
QString Welsegs::createSegmentEntry(SegmentedWell::Segment segment) {
 /*
  * 0. Segment number (first in range).
  * 1. Segment number (last in range).
  * 2. Branch number.
  * 3. Outlet segment number.
  * 4. Length of segment.
  * 5. Depth change through segment.
  * 6. Diameter.
  * 7. Roughness.
  * 8. * Cross sectional area for fluid flow.
  */
    auto record = GetBaseEntryLine(9);
    record[0] = segment.Index();
    record[1] = segment.Index();
    record[2] = segment.Branch();
    record[3] = segment.Outlet();
    record[4] = segment.Length();
    record[5] = segment.TVDChange();
    record[6] = segment.Diameter();
    record[7] = segment.Roughness();
    return record.join("  ");
}
}
}
