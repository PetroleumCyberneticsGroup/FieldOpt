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
#ifndef RUNTIMESETTINGS_H
#define RUNTIMESETTINGS_H

#include <boost/program_options.hpp>
#include <iostream>
#include <QString>
#include <QVector>
#include <QPair>
#include <stdexcept>
#include "Utilities/filehandling.hpp"
namespace po = boost::program_options;

namespace Runner {

/*!
 * \brief The RuntimeSettings class Parses, validates and stores any settings passed to FieldOpt through the command line.
 *
 * Boost's program options library is used.
 */
class RuntimeSettings
{
 public:
  RuntimeSettings(int argc, const char *argv[]);

  /*!
   * \brief The RunnerType enum lists the names of available runners.
   */
  enum RunnerType { SERIAL, ONEOFF, MPISYNC };

  QString driver_file() const { return driver_file_; }
  QString output_dir() const { return output_dir_; }
  QString simulator_driver_path() const { return simulator_driver_path_; }
  QString grid_file_path() const { return grid_file_path_; }
  QString simulator_exec_script_path() const { return simulator_exec_script_path_; }
  QString fieldopt_build_dir() const { return fieldopt_build_dir_; }
  int verbosity_level() const { return verbosity_level_; }
  bool overwrite_existing() const { return overwrite_existing_; }
  int max_parallel_sims() const { return max_parallel_sims_; }
  int threads_per_sim() const { return threads_per_sim_; }
  int simulation_timeout() const { return simulation_timeout_; }
  RunnerType runner_type() const { return runner_type_; }
  QPair<QVector<double>, QVector<double>> prod_coords() const { return prod_coords_; }
  QPair<QVector<double>, QVector<double>> inje_coords() const { return inje_coords_; }

 private:
  std::string str_out; //!< Temporary variable for verbosity function
  QString driver_file_; //!< Path to the driver file to be used by FieldOpt.
  QString output_dir_; //!< Directory in which to write all output.
  QString simulator_driver_path_; //!< Path to simulator driver file.
  QString grid_file_path_; //!< Path to reservoir grid file.
  QString simulator_exec_script_path_; //!< Path to script that launches the simulator.
  QString fieldopt_build_dir_; //!< Directory in which FieldOpt is built.
  int verbosity_level_; //!< Verbose mode (i.e. whether or not to print detailed/debug/diagnostic info to the console while running).
  bool overwrite_existing_; //!< Whether or not files in the specified output directory should be overwritten (only relevant if the directory is not empty).
  int max_parallel_sims_; //!< Maximum number of parallel simulations to start. This is important to define if you for example have a limited number of simulator licenses.
  int threads_per_sim_; //!< Number of threads to be used pr. simulation. Only works for ADGPRS.
  int simulation_timeout_; //!< Simulations will be terminated after running for simulation_timeout_ times the lowest recorded simulation time up to that point.
  RunnerType runner_type_; //!< The type of runner to be used (e.g. serial or parallel).
  QPair<QVector<double>, QVector<double>> prod_coords_; //!< The spline coordinates for the production well
  QPair<QVector<double>, QVector<double>> inje_coords_; //!< The spline coordinates for the injection well

  QString runnerTypeString() const; //!< Get a string representation of the runner type (used when printing settings to the terminal).
  QString wellSplineCoordinateString(const QPair<QVector<double>, QVector<double>> spline) const;

  po::variables_map createVariablesMap(int argc, const char *argv[]);
};

}

#endif // RUNTIMESETTINGS_H
