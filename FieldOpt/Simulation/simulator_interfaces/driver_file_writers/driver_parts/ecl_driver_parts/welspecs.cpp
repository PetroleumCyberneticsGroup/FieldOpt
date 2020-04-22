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

#include "welspecs.h"
#include <iostream>

namespace Simulation {
namespace ECLDriverParts {

Welspecs::Welspecs(QList<Model::Wells::Well *> *wells)
{
    initializeBaseEntryLine(10);
    head_ = "WELSPECS";
    foot_ = "/\n\n";
    for (int i = 0; i < wells->size(); ++i) {
        if (wells->at(i)->trajectory()->GetWellBlocks()->size() > 0)
            entries_.append(createWellEntry(wells->at(i)));
    }
}

Welspecs::Welspecs(QList<Model::Wells::Well *> *wells, int timestep) {
    initializeBaseEntryLine(10);
    head_ = "WELSPECS";
    foot_ = "/\n\n";
    for (auto well : *wells) {
        if (well->controls()->first()->time_step() == timestep && well->trajectory()->GetWellBlocks()->size() > 0) {
            entries_.append(createWellEntry(well));
        }
    }
}

QString Welspecs::GetPartString() const {
    // Return an empty string if there are no entries (at the timestep)
    if (entries_.size() == 0){
        return "";
    }

    QString entries = head_ + "\n";
    for (QStringList entry : entries_) {
        entries.append("    " + entry.join(" ") + " /\n");
    }
    entries.append("\n" + foot_);
    return entries;
}

QStringList Welspecs::createWellEntry(Model::Wells::Well *well)
{
    QStringList entry = QStringList(base_entry_line_);
    entry[0] = well->name();
    if (well->group().length() >= 1)
        entry[1] = well->group();
    entry[2] = QString::number(well->heel_i());
    entry[3] = QString::number(well->heel_j());

    switch (well->preferred_phase()) {
        case Settings::Model::PreferredPhase::Oil:
            entry[5] = "OIL";
            break;
        case Settings::Model::PreferredPhase::Water:
            entry[5] = "WATER";
            break;
        case Settings::Model::PreferredPhase::Gas:
            entry[5] = "GAS";
            break;
        case Settings::Model::PreferredPhase::Liquid:
            entry[5] = "LIQ";
            break;
        default:
            throw std::runtime_error("The preferred phase was not recognized for well " + well->name().toStdString());
    }
    return entry;
}


}
}
