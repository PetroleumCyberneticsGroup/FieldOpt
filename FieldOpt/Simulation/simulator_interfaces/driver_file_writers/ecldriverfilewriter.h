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

#ifndef ECLDRIVERFILEWRITER_H
#define ECLDRIVERFILEWRITER_H

#include "Settings/settings.h"
#include "Settings/simulator.h"
#include "Model/model.h"

namespace Simulation {
namespace SimulatorInterfaces {
class ECLSimulator;
}
}

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {

/*!
 * \brief The EclDriverFileWriter class writes driver files
 * that can be executed by the ECL100 reservoir simulator.
 * This class should _only_ be used by the ECLSimulator
 * class.
 *
 */
class EclDriverFileWriter
{
 private:
  friend class ::Simulation::SimulatorInterfaces::ECLSimulator;
  EclDriverFileWriter(::Settings::Settings *settings, Model::Model *model);
  void WriteDriverFile();

  Model::Model *model_;
  ::Settings::Settings *settings_;

  /// The content of the original (input) driver file.
  QStringList *original_driver_file_contents_;

  /// Path to the driver file to be written.
  QString output_driver_file_name_;
};

}
}
}

#endif // ECLDRIVERFILEWRITER_H
