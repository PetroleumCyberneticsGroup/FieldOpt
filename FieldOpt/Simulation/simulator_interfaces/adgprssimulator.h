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
#ifndef ADGPRSSIMULATOR_H
#define ADGPRSSIMULATOR_H
#include "simulator.h"

#include "Model/model.h"
#include "Settings/settings.h"
#include "driver_file_writers/adgprsdriverfilewriter.h"

namespace Simulation {
namespace SimulatorInterfaces {

/*!
 * \brief The AdgprsSimulator class implements simulation of models using the AD-GPRS reservoir simulator.
 *
 * This class should not be used directly except for instantiation. All other actions should be
 * called through the Simulator class. The intended use is as follows:
 *
 * \code
 *  Simulator sim = new AdgprsSimulator();
 *  sim.SetOutputDirectory("some/path");
 *  sim.Evaluate();
 *  sim.CleanUp();
 * \endcode
 *
 * \todo Support custom execution commands.
 */
class AdgprsSimulator : public Simulator
{
 public:
  AdgprsSimulator(Settings::Settings *settings, Model::Model *model);

  // Simulator interface
 public:
  void Evaluate();
  void CleanUp();
  virtual bool Evaluate(int timeout, int threads=1) override;

 private:
  QString initial_driver_file_parent_dir_path_;
  QString output_h5_summary_file_path_;
  Simulation::SimulatorInterfaces::DriverFileWriters::AdgprsDriverFileWriter *driver_file_writer_;
  void copyDriverFiles(); //!< Copy the original driver files.
  void verifyOriginalDriverFileDirectory(); //!< Ensure that all necessary files are present in the original dir.

  // Simulator interface
 protected:
  void UpdateFilePaths();

  // Simulator interface
 public:
  QString GetCompdatString();
};

}
}

#endif // ADGPRSSIMULATOR_H
