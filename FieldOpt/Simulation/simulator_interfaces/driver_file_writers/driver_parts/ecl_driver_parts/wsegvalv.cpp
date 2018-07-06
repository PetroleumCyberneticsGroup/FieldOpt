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

#include "wsegvalv.h"

namespace Simulation {
namespace ECLDriverParts {

Wsegvalv::Wsegvalv(Well *well) {
    head_ = "WSEGVALV\n";
    foot_ = "/\n\n";
    auto isegs = well->GetICDSegments();
    for (int i = 0; i < isegs.size(); ++i) {
        entries_.push_back(generateEntry(isegs[i], well->name()));
    }

}

Wsegvalv::Wsegvalv(QList<Model::Wells::Well *> *wells, int ts) {
    for (Well *well : *wells) {
        if (well->IsSegmented() && well->controls()->first()->time_step() == ts) {
            auto isegs = well->GetICDSegments();
            for (int i = 1; i < isegs.size(); ++i) {
                entries_.push_back(generateEntry(isegs[i], well->name()));
            }
        }
    }
}


QString Wsegvalv::GetPartString() const {
    if (entries_.size() == 0)
        return "";
    QString keyword = head_ + "\n";
    keyword += "\t" + entries_.join("\t") + "\n";
    keyword += foot_;
}

QString Wsegvalv::generateEntry(Segment seg, QString wname) {
/*!
 * 0. Well name.
 * 1. Segment number.
 * 2. Dimensionless flow coefficient (\$ C_v \$).
 * 3. Cross-section area for flow in the constriction (\$ A_c \$).
 */
    auto entry = GetBaseEntryLine(4);
    entry[0] = wname;
    entry[1] = seg.Index();
    entry[2] = seg.ParentICD()->flowCoefficient();
    entry[3] = seg.ParentICD()->valveSize();
    return "\t" + entry.join("  ") + "/";
}
}
}
