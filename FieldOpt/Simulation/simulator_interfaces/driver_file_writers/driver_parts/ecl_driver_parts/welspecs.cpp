/******************************************************************************
 *
 *
 *
 * Created: 12.11.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
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

#include "welspecs.h"
#include <stdexcept>

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {

Welspecs::Welspecs(QList<Model::Wells::Well *> *wells)
{
    initializeBaseEntryLine(17);
    head_ = "WELSPECS";
    foot_ = "/\n\n";
    for (int i = 0; i < wells->size(); ++i) {
        if (wells->at(i)->trajectory()->GetWellBlocks()->size() > 0)
            entries_.append(createWellEntry(wells->at(i)));
    }
}

QString Welspecs::GetPartString()
{
    QString entries = head_ + "\n";
    foreach (QStringList entry, entries_) {
        entries.append("    " + entry.join(" ") + " /\n");
    }
    entries.append("\n" + foot_);
    return entries;
}

QStringList Welspecs::createWellEntry(Model::Wells::Well *well)
{
    QStringList entry = QStringList(base_entry_line_);
    entry[0] = well->name();
    entry[2] = QString::number(well->heel_i());
    entry[3] = QString::number(well->heel_j());

    switch (well->prefered_phase()) {
    case Utilities::Settings::Model::PreferedPhase::Oil:
        entry[5] = "OIL";
        break;
    case Utilities::Settings::Model::PreferedPhase::Water:
        entry[5] = "WATER";
        break;
    case Utilities::Settings::Model::PreferedPhase::Gas:
        entry[5] = "GAS";
        break;
    case Utilities::Settings::Model::PreferedPhase::Liquid:
        entry[5] = "LIQ";
        break;
    default:
        throw std::runtime_error("The prefered phase was not recognized for well " + well->name().toStdString());
    }
    return entry;
}


}
}
}
}
}
