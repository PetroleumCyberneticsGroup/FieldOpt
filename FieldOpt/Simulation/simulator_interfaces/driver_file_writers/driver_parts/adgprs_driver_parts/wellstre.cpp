/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
   along with FieldOpt. If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "wellstre.h"

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace AdgprsDriverParts {

Wellstre::Wellstre(QList<Model::Wells::Well *> *wells,
                   Settings::Simulator::SimulatorFluidModel fluid_model)
{
    wells_ = wells;
    fluid_model_ = fluid_model;
}

QString Wellstre::GetPartString()
{
    return createKeyword();
}

QString Wellstre::createKeyword()
{
    QString wellstre = "WELLSTRE\n";
    for (int i = 0; i < wells_->length(); ++i) {
        if (wells_->at(i)->type() == Settings::Model::WellType::Injector) {
            QString line = QString("\t%1 /\n").arg(createWellEntry(wells_->at(i)));
            wellstre.append(line);
        }
    }
    wellstre.append("/\n");
    return wellstre;
}

QString Wellstre::createWellEntry(Model::Wells::Well *well)
{
    // Return an empty string if the well is not an injector
    if (well->type() != Settings::Model::WellType::Injector)
        return "";

    if (fluid_model_ == Settings::Simulator::SimulatorFluidModel::BlackOil) {
        initializeBaseEntryLine(4);
        base_entry_line_[0] = well->name();
        base_entry_line_[1] = "0";
        base_entry_line_[2] = "0";
        base_entry_line_[3] = "0";

        switch (well->preferred_phase()) {
            case Settings::Model::PreferredPhase::Water:
                base_entry_line_[3] = "1";
                break;
            case Settings::Model::PreferredPhase::Gas:
                base_entry_line_[1] = "1";
                break;
            case Settings::Model::PreferredPhase::Oil:
                base_entry_line_[2] = "1";
                break;
            default:
                base_entry_line_[3] = "1";
                break;
        }
    }
    else { // If its not a black oil model, it must be a dead oil model
        initializeBaseEntryLine(3);
        base_entry_line_[0] = well->name();
        base_entry_line_[1] = "0";
        base_entry_line_[2] = "0";

        switch (well->preferred_phase()) {
            case Settings::Model::PreferredPhase::Water:
                base_entry_line_[1] = "1";
                break;
            case Settings::Model::PreferredPhase::Oil:
                base_entry_line_[2] = "1";
                break;
            default:
                base_entry_line_[1] = "1";
                break;
        }
    }

    return base_entry_line_.join("\t");
}

}
}
}
}
}
