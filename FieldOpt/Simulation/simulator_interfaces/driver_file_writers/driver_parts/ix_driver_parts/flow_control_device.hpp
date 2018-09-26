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

#ifndef FIELDOPT_FLOW_CONTROL_DEVICE_HPP
#define FIELDOPT_FLOW_CONTROL_DEVICE_HPP

#include <string>
#include "Model/wells/well.h"

namespace Simulation {
namespace IXParts {

inline std::string AllWellDevices(Model::Wells::Well *well) {
    std::stringstream devices;
    for (auto icv : well->GetSimpleICDs()) {
        devices << "TIME " << icv.timeStep()                                << std::endl;
        devices << "Well \"" << well->name().toStdString() << "\" {"        << std::endl;
        devices << "    FlowControlDevice \"" << icv.deviceName() << "\" {" << std::endl;
        devices << "        Setting=" << icv.setting()                      << std::endl;
        if (icv.setting() >= 0.0) {
            devices << "        Status=OPEN" << std::endl;
        }
        else {
            devices << "        Status=CLOSED" << std::endl;
        }
        devices << "    }"                                                  << std::endl;
        devices << "}"                                                      << std::endl;
    }
    return devices.str();
}
}
}

#endif //FIELDOPT_FLOW_CONTROL_DEVICE_HPP
