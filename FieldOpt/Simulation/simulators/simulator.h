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
#include "Utilities/settings/settings.h"
#include "Utilities/settings/simulator.h"

namespace Simulation {
namespace Simulator {

/*!
 * \brief The Simulator class acts as an interface for all reservoir simulators.
 */
class Simulator {
public:
    virtual ~Simulator();

    /*!
     * \brief Evaluate Executes a simulation of the specified model.
     */
    virtual void Evaluate(Model::Model *model) = 0;

    /*!
     * \brief CleanUp Perform cleanup after simulation, i.e. delete output files.
     */
    virtual void CleanUp() = 0;

    /*!
     * \brief SetOutputDirectory Set the directory in which to execute the simulation.
     */
    virtual void SetOutputDirectory(QString output_directory);

protected:
    Simulator(Utilities::Settings::Settings settings);

    QString initial_driver_file_path_;
    QString output_directory_;
};

}
}

#endif // SIMULATOR

