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

Compsegs::Compsegs(Well *well) {
    head_ = "COMPSEGS\n";
    foot_ = "/\n\n";
    auto asegs = well->GetAnnulusSegments();
    CompsegsKeyword kw;
    kw.wname = well->name();
    for (int i = 0; i < asegs.size(); ++i) {
        kw.entries.push_back(generateEntry(asegs[i]));
    }
    keywords_.push_back(kw);
}
Compsegs::Compsegs(QList<Model::Wells::Well *> *wells, int ts) {
    for (Well *well : *wells) {
        if (well->IsSegmented() && well->controls()->first()->time_step() == ts) {
            CompsegsKeyword kw;
            kw.wname = well->name();
            auto asegs = well->GetAnnulusSegments();
            for (int i = 0; i < asegs.size(); ++i) {
                kw.entries.push_back(generateEntry(asegs[i]));
            }
            keywords_.push_back(kw);
        }
    }
}
QString Simulation::ECLDriverParts::Compsegs::GetPartString() const {
    if (keywords_.size() == 0)
        return "";

    QString all_keywords = "";
    for (auto kw : keywords_) {
        all_keywords += kw.buildKeyword();
    }
    return all_keywords;
}
QString Compsegs::generateEntry(Segment seg) {
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
    entry[0] = QString::number(seg.ParentBlock()->i());
    entry[1] = QString::number(seg.ParentBlock()->j());
    entry[2] = QString::number(seg.ParentBlock()->k());
    entry[3] = QString::number(seg.Branch());
    entry[4] = QString::number(seg.OutletMD());
    entry[5] = QString::number(seg.OutletMD() + seg.Length());
    return "\t" + entry.join("  ") + "  /";
}
QString Compsegs::CompsegsKeyword::buildKeyword() const {
    QString kw = "COMPSEGS\n";
    kw += "\t" + this->wname + "  /\n";
    kw += this->entries.join("\n");
    kw += "\n/\n\n";
    return kw;
}
}
}
