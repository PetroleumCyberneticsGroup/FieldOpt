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

#include "main_runner.h"
#include "serial_runner.h"
#include "oneoff_runner.h"
#include "synchronous_mpi_runner.h"

namespace Runner {

MainRunner::MainRunner(RuntimeSettings *rts)
{
    runtime_settings_ = rts;

    switch (runtime_settings_->runner_type()) {
        case RuntimeSettings::RunnerType::SERIAL:
            runner_ = new SerialRunner(runtime_settings_);
            break;
        case RuntimeSettings::RunnerType::ONEOFF:
            runner_ = new OneOffRunner(runtime_settings_);
            break;
        case RuntimeSettings::RunnerType::MPISYNC:
            runner_ = new MPI::SynchronousMPIRunner(runtime_settings_);
            break;
        default:
            throw std::runtime_error("Runner type not recognized.");
    }
}

void MainRunner::Execute()
{
    if (runtime_settings_->verbosity_level()) {
        std::cout << "Starting optimization run." << std::endl;
    }
    runner_->Execute();
}

}
