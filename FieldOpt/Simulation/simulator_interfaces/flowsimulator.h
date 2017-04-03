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
#ifndef FIELDOPT_FLOWSIMULATOR_H
#define FIELDOPT_FLOWSIMULATOR_H

#include "simulator.h"
#include "driver_file_writers/flowdriverfilewriter.h"
#include "Model/model.h"

namespace Simulation {
namespace SimulatorInterfaces {
class FlowSimulator : public Simulator {
 public:
  FlowSimulator(Settings::Settings *settings, Model::Model *model);

  virtual void Evaluate() override;
  virtual bool Evaluate(int timeout, int threads=1) override;
  virtual void CleanUp() override;
  virtual void UpdateFilePaths() override;

 private:
  DriverFileWriters::FlowDriverFileWriter *driver_file_writer_;
  QString script_path_;
  QStringList script_args_;
  QString initial_driver_file_parent_dir_path_;

  void verifyOriginalDriverFileDirectory();
  void copyDriverFiles();
};
}
}


#endif //FIELDOPT_FLOWSIMULATOR_H
