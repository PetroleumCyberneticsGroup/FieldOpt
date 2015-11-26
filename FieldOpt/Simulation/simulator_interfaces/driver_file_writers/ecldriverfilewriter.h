/******************************************************************************
 *
 *
 *
 * Created: 12.11.2015 2015 by einar
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

#ifndef ECLDRIVERFILEWRITER_H
#define ECLDRIVERFILEWRITER_H

#include "Utilities/settings/settings.h"
#include "Utilities/settings/simulator.h"
#include "Model/model.h"

namespace Simulation { namespace SimulatorInterfaces {
    class ECLSimulator;
}}

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {

/*!
 * \brief The EclDriverFileWriter class writes driver files that can be executed
 * by the ECL100 reservoir simulator. This class should _only_ be used by the
 * ECLSimulator class.
 */
class EclDriverFileWriter
{
private:
    friend class ::Simulation::SimulatorInterfaces::ECLSimulator;
    EclDriverFileWriter(::Utilities::Settings::Settings *settings, Model::Model *model);
    void WriteDriverFile();

    Model::Model *model_;
    ::Utilities::Settings::Settings *settings_;
    QStringList *original_driver_file_contents_; //!< The content of the original (input) driver file.
    QString output_driver_file_name_; //!< Path to the driver file to be written.
};

}
}
}


#endif // ECLDRIVERFILEWRITER_H
