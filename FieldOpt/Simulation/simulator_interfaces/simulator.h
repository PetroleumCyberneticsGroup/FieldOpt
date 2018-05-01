/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef SIMULATOR
#define SIMULATOR

// ---------------------------------------------------------
#include <QString>

// -------------------------------------------------------
#include "Model/model.h"
#include "Simulation/results/results.h"
#include "Settings/settings.h"
#include "Settings/simulator.h"
#include "Simulation/execution_scripts/execution_scripts.h"
#include "Utilities/debug.hpp"

// ---------------------------------------------------------
namespace Simulation {
namespace SimulatorInterfaces {

// =========================================================
/*!
 * \brief The Simulator class acts as an
 * interface for all reservoir simulators.
 *
 * The constructor for this class is not intended to
 * be used directly. The intended use is as follows:
 *
 * \code
 *  Simulator sim = new ECLSimulator();
 *  sim.SetOutputDirectory("some/path");
 *  sim.Evaluate();
 *  sim.CleanUp();
 * \endcode
 */
class Simulator
{
 public:
  // -------------------------------------------------------
  /*!
   * \brief SetOutputDirectory Set the directory
   * in which to execute the simulation.
   */
  void SetOutputDirectory(QString output_directory);

  // -------------------------------------------------------
  /*!
   * \brief results Get the simulation results.
   */
  ::Simulation::Results::Results *results();

  // -------------------------------------------------------
  /*!
   * \brief Evaluate Writes the driver file
   * and executes a simulation of the model.
   */
  virtual void Evaluate() = 0;

  // -------------------------------------------------------
  /*!
   * \brief Evaluate Writes the driver file and executes a
   * simulation of the model. The simulation is terminated
   * after the amount of seconds provided in the timeout
   * argument.
   *
   * @param timeout Number of seconds before
   * the simulation should be terminated.
   *
   * @param threads Number of threads to be used
   * by the simulator. Only works for AD-GPRS.
   *
   * @return True if the simuation completes
   * before the set timeout, otherwise false.
   */
  virtual bool Evaluate(int timeout, int threads=1) = 0;

  // -------------------------------------------------------
  /*!
   * @brief Only write driver files; don't execute simulation.
   *
   * This should be used at the end of the run to write the
   * best case driver files and add the best case compdat
   * to the model.
   *
   * @return
   */
  virtual void WriteDriverFilesOnly() = 0;

  // -------------------------------------------------------
  /*!
   * \brief CleanUp Perform cleanup after
   * simulation, i.e. delete output files.
   */
  virtual void CleanUp() = 0;

  // -------------------------------------------------------
  void set_verbosity_vector(const vector<int> verb_vector)
  { verb_vector_ = verb_vector; }

  vector<int> verb_vector() const { return verb_vector_; }

 protected:
  // -------------------------------------------------------
  /*!
   * Set various path variables. Should
   * only be called by child classes.
   *
   * @param settings
   *
   * @return
   */
  Simulator(Settings::Settings *settings);

  // -------------------------------------------------------
  void updateResultsInModel();

  // -------------------------------------------------------
  QString build_dir_;

  // -------------------------------------------------------
  // DRIVER FILE VARIANTS (PATH + NAME)

  // Path to the driver file to be used as
  // a base for the generated driver files.
  QString initial_driver_file_path_;

  // The name of the driver main file.
  QString initial_driver_file_name_;

  // -------------------------------------------------------
  // PARENT DRIVER FILE VARIANTS (PATH + NAME)

  // Path to the directory containing
  // the main driver file.
  QString init_driver_file_parent_dir_path_;

  // The name of the directory containing
  // the initial main driver file.
  QString init_driver_file_parent_dir_name_;

  // -------------------------------------------------------
  // The directory in which to write new
  // driver files and execute simulations.
  QString output_directory_;

  QString initial_driver_file_path_opt_;

  // -------------------------------------------------------
  // SIM INCLUDE DIR VARIANTS (PATH + NAME)
  QString init_sim_incl_dir_path_;

  QString init_sim_incl_dir_name_;

  // -------------------------------------------------------
  // Path to the schedule file. This file will
  // be overwritten by FieldOpt when optimizing.
  QString initial_schedule_path_;

  // Path that should be used when writing new deck
  // parts and executing simulations. Should be inside
  // output dir.
  QString current_output_deck_parent_dir_path_;

  // -------------------------------------------------------
  QString script_path_;
  QStringList script_args_;

  // -------------------------------------------------------
  ::Simulation::Results::Results *results_;
  Settings::Settings *settings_;
  Model::Model *model_;

  QList<int> control_times_;
  virtual void UpdateFilePaths() = 0;

  // -------------------------------------------------------
  // Verbosity levels for runtime console logging.
  vector<int> verb_vector_ = vector<int>(11,1);
};

}
}

#endif // SIMULATOR

