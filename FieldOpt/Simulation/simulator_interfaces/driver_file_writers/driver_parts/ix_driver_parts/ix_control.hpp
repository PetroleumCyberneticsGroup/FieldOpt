/******************************************************************************
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@ntnu.no>
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

#ifndef FIELDOPT_IX_CONTROL_HPP
#define FIELDOPT_IX_CONTROL_HPP

#include "Model/wells/well.h"
namespace Simulation {
namespace IXParts {

inline std::string create_single_control(const QString &well_name, bool is_injector, const Model::Wells::Control *control) {
    std::string status = control->open() ? "OPEN" : "ALL_COMPLETIONS_SHUTIN";
    std::stringstream ss;
    ss << "TIME " << control->time_step()                                                      << std::endl;
    ss << "Well \"" << well_name.toStdString() << "\" {"                                       << std::endl;
    ss << "    Status = " << status                                                            << std::endl;
    ss << "    Constriants = ["                                                                << std::endl;
    if (is_injector) {
        if (control->mode() == Settings::Model::ControlMode::BHPControl) {
            ss << "        Constraint(" << control->bhp() << " INJECTION_BOTTOM_HOLE_PRESSURE)" << std::endl;
        }
        else if (control->mode() == Settings::Model::ControlMode::RateControl) {
            ss << "        Constraint(" << control->rate() << " WATER_INJECTION_RATE)"          << std::endl;
        }
    }
    else {
        if (control->mode() == Settings::Model::ControlMode::BHPControl) {
            ss << "        Constraint(" << control->bhp() << " BOTTOM_HOLE_PRESSURE)"           << std::endl;
        }
        else if (control->mode() == Settings::Model::ControlMode::RateControl) {
            ss << "        Constraint(" << control->rate() << " LIQUID_PRODCTION_RATE)"         << std::endl;
        }
    }
    ss << "    ]"                                                                              << std::endl;

    ss << "}"                                                                                  << std::endl;
    return ss.str();

}

inline std::string CreateControlEntries(Model::Wells::Well *well) {
    std::string ctrlstr= "";
    auto controls = well->controls();
    for (auto control : *controls) {
        ctrlstr += create_single_control(well->name(), well->IsInjector(), control) + "\n";
    }
    return ctrlstr;
}



}
}

#endif //FIELDOPT_IX_CONTROL_HPP
