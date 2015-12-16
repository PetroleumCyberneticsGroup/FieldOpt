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

class RuntimeSettings
{
public:
    RuntimeSettings(po::variables_map vm);
    enum RunnerType { SERIAL };

private:
    QString input_file_;
    QString output_dir_;
    bool show_progress_;
    bool verbose_;
    bool overwrite_existing_;
    int max_parallel_sims_;
    RunnerType runner_type_;

    QString runnerTypeString();
};

}

#endif // RUNTIMESETTINGS_H
