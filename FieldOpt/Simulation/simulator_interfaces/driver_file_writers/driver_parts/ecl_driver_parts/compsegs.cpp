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

#include "compsegs.h"

namespace Simulation {
namespace ECLDriverParts {

Compsegs::Compsegs(SegmentedWell *well) {
    head_ = "COMPSEGS\n";
    foot_ = "/\n\n";
    auto asegs = well->GetAnnulusSegments();
    wname_ = well->name();
    for (int i = 0; i < asegs.size(); ++i) {
        entries_.push_back(generateEntry(asegs[i]));
    }
}

QString Simulation::ECLDriverParts::Compsegs::GetPartString() const {
    QString keyword = head_ + "\n";
    keyword += "\t" + wname_ + "  /\n";
    keyword += entries_.join("\n") + "\n";
    keyword += foot_;
    return keyword;
}
QString Compsegs::generateEntry(SegmentedWell::Segment seg) {
/*
 * Record 1:
 *  1. Name of well.
 *
 * Remaining records:
 *  0. I-location
 *  1. J-location
 *  2. K-location
 *  3. Branch number.
 *  4. MD from root to _start_ of connection in this block.
 *  5. MD from root to _end_ of connection in this block.
 */
    auto entry = GetBaseEntryLine(6);
    entry[0] = seg.ParentBlock()->i();
    entry[1] = seg.ParentBlock()->j();
    entry[2] = seg.ParentBlock()->k();
    entry[3] = seg.Branch();
    entry[4] = seg.OutletMD();
    entry[5] = seg.OutletMD() + seg.Length();
    return "\t" + entry.join("  ") + "  /\n";
}
}
}
