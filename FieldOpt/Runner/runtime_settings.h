/******************************************************************************
 *
 *
 *
 * Created: 16.12.2015 2015 by einar
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

#ifndef RUNTIMESETTINGS_H
#define RUNTIMESETTINGS_H

#include <boost/program_options.hpp>
#include <iostream>
#include <QString>
#include <stdexcept>
#include "Utilities/file_handling/filehandling.h"

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
    RuntimeSettings(po::variables_map vm);

    /*!
     * \brief The RunnerType enum lists the names of available runners.
     */
    enum RunnerType { SERIAL };

    QString driver_file() const { return driver_file_; }
    QString output_dir() const { return output_dir_; }
    QString simulator_driver_path() const { return simulator_driver_path_; }
    QString grid_file_path() const { return grid_file_path_; }
    bool verbose() const { return verbose_; }
    bool overwrite_existing() const { return overwrite_existing_; }
    int max_parallel_sims() const { return max_parallel_sims_; }
    RunnerType runner_type() const { return runner_type_; }

private:
    QString driver_file_; //!< Path to the driver file to be used by FieldOpt.
    QString output_dir_; //!< Directory in which to write all output.
    QString simulator_driver_path_; //!< Path to simulator driver file.
    QString grid_file_path_; //!< Path to reservoir grid file.
    bool verbose_; //!< Verbose mode (i.e. whether or not to print detailed/debug/diagnostic info to the console while running).
    bool overwrite_existing_; //!< Whether or not files in the specified output directory should be overwritten (only relevant if the directory is not empty).
    int max_parallel_sims_; //!< Maximum number of parallel simulations to start. This is important to define if you for example have a limited number of simulator licenses.
    RunnerType runner_type_; //!< The type of runner to be used (e.g. serial or parallel).

    QString runnerTypeString(); //!< Get a string representation of the runner type (used when printing settings to the terminal).
};

}

#endif // RUNTIMESETTINGS_H
