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
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <Utilities/printer.hpp>
#include "simulator.h"
#include "settings_exceptions.h"
#include "Utilities/filehandling.hpp"

using namespace Utilities::FileHandling;

namespace Settings {

Simulator::Simulator(QJsonObject json_simulator, Paths &paths)
{
    setPaths(json_simulator, paths);
    setType(json_simulator);
    setParams(json_simulator);
    setCommands(json_simulator);
    setFluidModel(json_simulator);
}

void Simulator::setPaths(QJsonObject json_simulator, Paths &paths) {
    if (!paths.IsSet(Paths::ENSEMBLE_FILE)) {
        is_ensemble_ = false;
        if (!paths.IsSet(Paths::SIM_DRIVER_FILE) && json_simulator.contains("DriverPath")) {
            paths.SetPath(Paths::SIM_DRIVER_FILE, json_simulator["DriverPath"].toString().toStdString());
        }
        paths.SetPath(Paths::SIM_DRIVER_DIR, GetParentDirectoryPath(paths.GetPath(Paths::SIM_DRIVER_FILE)));

        if (!paths.IsSet(Paths::SIM_SCH_FILE) && json_simulator.contains("ScheduleFile")) {
            auto schedule_path = paths.GetPath(
                Paths::SIM_DRIVER_DIR) + "/"
                + json_simulator["ScheduleFile"].toString().toStdString();
            paths.SetPath(Paths::SIM_SCH_FILE, schedule_path);
        }
    }
    else { // This is an ensemble run
        is_ensemble_ = true;
        ensemble_ = Ensemble(paths.GetPath(Paths::ENSEMBLE_FILE));
        // Set the data file path to the first realization so that the deck parser can find it
        paths.SetPath(Paths::SIM_DRIVER_FILE, ensemble_.GetRealization(ensemble_.GetAliases()[0]).data());
    }
}

void Simulator::setType(QJsonObject json_simulator) {
    QString type = json_simulator["Type"].toString();
    if (QString::compare(type, "ECLIPSE") == 0)
        type_ = SimulatorType::ECLIPSE;
    else if (QString::compare(type, "ADGPRS") == 0)
        type_ = SimulatorType::ADGPRS;
    else if (QString::compare(type, "Flow", Qt::CaseInsensitive) == 0)
        type_ = SimulatorType::Flow;
    else if (QString::compare(type, "IX", Qt::CaseInsensitive) == 0)
        type_ = SimulatorType::INTERSECT;
    else throw SimulatorTypeNotRecognizedException(
            "The simulator type " + type.toStdString() + " was not recognized");
}

void Simulator::setParams(QJsonObject json_simulator) {
    if (json_simulator.contains("MaxMinutes") && json_simulator["MaxMinutes"].toInt() > 0) {
        max_minutes_ = json_simulator["MaxMinutes"].toInt();
    }
    else {
        max_minutes_ = -1;
    }
}

void Simulator::setCommands(QJsonObject json_simulator) {
    QJsonArray commands = json_simulator["Commands"].toArray();
    script_name_ = "";
    if (json_simulator.contains("ExecutionScript") && json_simulator["ExecutionScript"].toString().size() > 0) {
        script_name_ = json_simulator["ExecutionScript"].toString();
    }
    if (json_simulator.contains("Commands") && commands.size() > 0) {
        commands_ = new QStringList();
        for (int i = 0; i < commands.size(); ++i) {
            commands_->append(commands[i].toString());
        }
    }
    if (script_name_.length() == 0 && commands.size() == 0)
        Printer::ext_warn("No simulator commands or scripts given in driver file. "
                          "Relying on script path being passed as runtime argument.", "Settings", "Simulator");
}

void Simulator::setFluidModel(QJsonObject json_simulator) {
    if (json_simulator.contains("FluidModel")) {
        QString fluid_model = json_simulator["FluidModel"].toString();
        if (QString::compare(fluid_model, "DeadOil") == 0)
            fluid_model_ = SimulatorFluidModel::DeadOil;
        else if (QString::compare(fluid_model, "BlackOil") == 0)
            fluid_model_ = SimulatorFluidModel::BlackOil;
    }
    else fluid_model_ = SimulatorFluidModel::BlackOil;
}

}
