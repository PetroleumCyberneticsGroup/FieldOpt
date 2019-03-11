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

#ifndef FIELDOPT_PATHS_H
#define FIELDOPT_PATHS_H

#include "Utilities/filehandling.hpp"

using namespace Utilities::FileHandling;

class Paths {
 public:
  Paths();

  /*!
   * Path enum. Directory paths should have values < 0; files >= 0.
   */
  enum Path : int {DRIVER_FILE=0, SIM_DRIVER_FILE=1, GRID_FILE=2, SIM_EXEC_SCRIPT_FILE=3,
    SIM_SCH_FILE=4, SIM_OUT_DRIVER_FILE=5, SIM_OUT_SCH_FILE=6, SIM_HDF5_FILE=7,
    ENSEMBLE_FILE=8, SIM_SCH_INSET_FILE=9,
    BUILD_DIR=-1,  OUTPUT_DIR=-2, SIM_DRIVER_DIR=-3, SIM_WORK_DIR=-4,
    SIM_AUX_DIR=-5, TRAJ_DIR=-6
  };

  const std::string &GetPathDescription(Path path) const;

  void SetPath(Path path, std::string path_string);

  bool IsSet(Path path);

  std::string GetPath(Path path);

 private:
  std::map<Path, std::string> path_descriptions = {
      std::pair<Path, std::string> {DRIVER_FILE, "Driver file"},
      std::pair<Path, std::string> {SIM_DRIVER_FILE, "Simulator driver file"},
      std::pair<Path, std::string> {GRID_FILE, "Grid file"},
      std::pair<Path, std::string> {SIM_EXEC_SCRIPT_FILE, "Simulator execution script"},
      std::pair<Path, std::string> {SIM_SCH_FILE, "Simulator schedule section file"},
      std::pair<Path, std::string> {SIM_OUT_DRIVER_FILE, "Simulation output driver file"},
      std::pair<Path, std::string> {SIM_OUT_SCH_FILE, "Simulation output schedule file"},
      std::pair<Path, std::string> {SIM_SCH_INSET_FILE, "Simulator schedule inset file"},
      std::pair<Path, std::string> {SIM_HDF5_FILE, "HDF5 Summary file"},
      std::pair<Path, std::string> {ENSEMBLE_FILE, "Ensemble description file"},
      std::pair<Path, std::string> {BUILD_DIR, "Build directory"},
      std::pair<Path, std::string> {OUTPUT_DIR, "Output directory"},
      std::pair<Path, std::string> {SIM_DRIVER_DIR, "Simulation driver parent directory"},
      std::pair<Path, std::string> {SIM_WORK_DIR, "Simulation work directory"},
      std::pair<Path, std::string> {SIM_AUX_DIR, "Auxilary files for simulation directory"},
      std::pair<Path, std::string> {TRAJ_DIR, "Directory contaning trajectory files for import"}
  };

  std::map<Path, std::string> paths_;
};



#endif //FIELDOPT_PATHS_H
