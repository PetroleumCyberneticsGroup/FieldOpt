/******************************************************************************
 *
 *
 *
 * Created: 15.10.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
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

#ifndef SIMULATOR
#define SIMULATOR

#include <QString>
#include "Model/model.h"
#include "Simulation/results/results.h"
#include "Utilities/settings/settings.h"
#include "Utilities/settings/simulator.h"
#include "Simulation/execution_scripts/execution_scripts.h"

namespace Simulation {
namespace SimulatorInterfaces {

/*!
 * \brief The Simulator class acts as an interface for all reservoir simulators.
 *
 * The constructor for this class is not intended to be used directly. The intended use
 * is as follows:
 *
 * \code
 *  Simulator sim = new ECLSimulator();
 *  sim.SetOutputDirectory("some/path");
 *  sim.Evaluate();
 *  sim.CleanUp();
 * \endcode
 */
class Simulator {
public:
    /*!
     * \brief SetOutputDirectory Set the directory in which to execute the simulation.
     */
    void SetOutputDirectory(QString output_directory);

    /*!
     * \brief results Get the simulation results.
     */
    ::Simulation::Results::Results *results();

    /*!
     * \brief Evaluate Writes the driver file and executes a simulation of the model.
     */
    virtual void Evaluate() = 0;

    /*!
     * \brief CleanUp Perform cleanup after simulation, i.e. delete output files.
     */
    virtual void CleanUp() = 0;

protected:
    /*!
     * \brief Simulator This constructor should only be called by child classes.
     */
    Simulator(){}

    QString initial_driver_file_path_; //!< Path to the driver file to be used as a base for the generated driver files.
    QString output_directory_; //!< The directory in which to write new driver files and execute simulations.

    ::Simulation::Results::Results *results_;
    Utilities::Settings::Settings *settings_;
    Model::Model *model_;
    virtual void UpdateFilePaths() = 0;
};

}
}

#endif // SIMULATOR

