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

#ifndef WELLSTRE_H
#define WELLSTRE_H

#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/ecldriverpart.h"
#include "Model/wells/well.h"
#include "Settings/simulator.h"
#include <QStringList>

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace AdgprsDriverParts {

class Wellstre : public ECLDriverParts::ECLDriverPart
{
 public:
  Wellstre(QList<Model::Wells::Well *> *wells,
           Settings::Simulator::SimulatorFluidModel fluid_model);

  // DriverPart interface
 public:
  QString GetPartString();

 private:
  QString createKeyword();
  QString createWellEntry(Model::Wells::Well *well);
  QList<Model::Wells::Well *> *wells_;
  Settings::Simulator::SimulatorFluidModel fluid_model_;
};

}
}
}
}
}

#endif // WELLSTRE_H
