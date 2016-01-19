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
#include "Optimization/objective/weightedsum.h"
#include "Simulation/simulator_interfaces/eclsimulator.h"

namespace Runner {

AbstractRunner::AbstractRunner(RuntimeSettings *runtime_settings)
{
    runtime_settings_ = runtime_settings;
    settings_ = new Utilities::Settings::Settings(runtime_settings->driver_file(), runtime_settings->output_dir());
    settings_->set_verbosity(runtime_settings_->verbose());
    model_ = new Model::Model(*settings_->model());

    // Initialize simulator
    switch (settings_->simulator()->type()) {
    case ::Utilities::Settings::Simulator::SimulatorType::ECLIPSE:
        if (runtime_settings_->verbose()) std::cout << "Using ECL100 reservoir simulator." << std::endl;
        simulator_ = new Simulation::SimulatorInterfaces::ECLSimulator(settings_, model_);
        break;
    default:
        throw std::runtime_error("Unable to initialize runner: simulator set in driver file not recognized.");
    }

    // Evaluate the base case if not already done.
    if (!simulator_->results()->isAvailable()) {
        if (runtime_settings_->verbose()) std::cout << "Simulating base case." << std::endl;
        simulator_->Evaluate();
    }

    // Initialize objective function
    switch (settings_->optimizer()->type()) {
    case Utilities::Settings::Optimizer::ObjectiveType::WeightedSum:
        if (runtime_settings->verbose()) std::cout << "Using WeightedSum-type objective function." << std::endl;
        objective_function_ = new Optimization::Objective::WeightedSum(settings_->optimizer(), simulator_->results());
        break;
    default:
        throw std::runtime_error("Unable to initialize runner: objective function type not recognized.");
    }

    base_case_ = new Optimization::Case(model_->variables()->GetBinaryVariableValues(),
                                        model_->variables()->GetDiscreteVariableValues(),
                                        model_->variables()->GetContinousVariableValues());
    base_case_->set_objective_function_value(objective_function_->value());
    if (runtime_settings_->verbose()) std::cout << "Base case objective function value set to: " << base_case_->objective_function_value() << std::endl;

    // Initialize optimizer
    switch (settings_->optimizer()->type()) {
    case Utilities::Settings::Optimizer::OptimizerType::Compass:
        if (runtime_settings_->verbose()) std::cout << "Using CompassSearch optimization algorithm." << std::endl;
        optimizer_ = new Optimization::Optimizers::CompassSearch(settings_->optimizer(), base_case_, model_->variables());
        break;
    default:
        throw std::runtime_error("Unable to initialize runner: optimization algorithm set in driver file not recognized.");
        break;
    }

    bookkeeper_ = new Bookkeeper(settings_, optimizer_->case_handler());
}

}
