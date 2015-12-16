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

#include "abstract_runner.h"
#include "Optimization/optimizers/compass_search.h"
#include "Simulation/simulator_interfaces/eclsimulator.h"

namespace Runner {
AbstractRunner::AbstractRunner()
{

}

AbstractRunner::AbstractRunner(RuntimeSettings *runtime_settings)
{
    runtime_settings_ = runtime_settings;
    settings_ = new Utilities::Settings::Settings(runtime_settings->driver_file());
    model_ = new Model::Model(settings_->model());
    base_case_ = new Optimization::Case(model_->variables()->GetBinaryVariables(),
                                        model_->variables()->GetDiscreteVariables(),
                                        model_->variables()->GetContinousVariables());

    // Initialize optimizer
    switch (settings_->optimizer()->type()) {
    case Utilities::Settings::Optimizer::OptimizerType::Compass:
        optimizer_ = new Optimization::Optimizers::CompassSearch(settings_->optimizer(), base_case_, model_->variables());
        break;
    default:
        throw std::runtime_error("Unable to initialize runner: optimization algorithm set in driver file not recognized.");
        break;
    }

    // Initialize simulator
    switch (settings_->simulator()) {
    case ::Utilities::Settings::Simulator::SimulatorType::ECLIPSE:
        simulator_ = new Simulation::SimulatorInterfaces::ECLSimulator(settings_->simulator(), model_);
        break;
    default:
        throw std::runtime_error("Unable to initialize runner: simulator set in driver file not recognized.");
        break;
    }
}

}
