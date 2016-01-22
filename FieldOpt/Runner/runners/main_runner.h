/******************************************************************************
 *
 *
 *
 * Created: 14.12.2015 2015 by einar
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

#ifndef RUNNER_H
#define RUNNER_H

#include <stdexcept>
#include "runtime_settings.h"
#include "abstract_runner.h"

namespace Runner {

/*!
 * \brief The MainRunner class takes care of initializing and starting the actual runner indicated in the runtime settings.
 */
class MainRunner
{
public:
    MainRunner(RuntimeSettings *rts);

    /*!
     * \brief Execute Start the optimization run by calling the Execute function in the simulator.
     */
    void Execute();

private:
    RuntimeSettings *runtime_settings_;
    AbstractRunner *runner_;
};

}

#endif // RUNNER_H
