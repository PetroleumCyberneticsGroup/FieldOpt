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

#include <Simulation/simulator_interfaces/flowsimulator.h>
#include <Optimization/optimizers/APPS.h>
#include <Optimization/optimizers/GeneticAlgorithm.h>
#include <Optimization/optimizers/RGARDD.h>
#include "abstract_runner.h"
#include "Optimization/optimizers/compass_search.h"
#include "Optimization/optimizers/ExhaustiveSearch2DVert.h"
#include "Optimization/objective/weightedsum.h"
#include "Simulation/simulator_interfaces/eclsimulator.h"
#include "Simulation/simulator_interfaces/adgprssimulator.h"
#include "Utilities/math.hpp"

using std::cout;

namespace Runner {

AbstractRunner::AbstractRunner(RuntimeSettings *runtime_settings)
{
    runtime_settings_ = runtime_settings;
    settings_ = 0;
    model_ = 0;
    simulator_ = 0;
    objective_function_ = 0;
    base_case_ = 0;
    optimizer_ = 0;
    bookkeeper_ = 0;
}

double AbstractRunner::sentinelValue() const
{
    if (settings_->optimizer()->mode() == Settings::Optimizer::OptimizerMode::Minimize)
        return -1*sentinel_value_;
    return sentinel_value_;
}

void AbstractRunner::InitializeSettings(QString output_subdirectory)
{
    QString output_directory = runtime_settings_->output_dir();
    if (output_subdirectory.length() > 0) { // if mpirunner
        output_directory.append(QString("/%1/").arg(output_subdirectory));
    }

    Utilities::FileHandling::CreateDirectory(output_directory);
    QString include_directory = runtime_settings_->include_dir();

    settings_ = new Settings::Settings(runtime_settings_->driver_file(),
                                       output_directory, include_directory);
    settings_->set_verbosity_level(runtime_settings_->verbosity_level());

    if (output_subdirectory.length() > 0) { // if mpirunner
        settings_->set_mpirunner_subdirectory("../");
    }

    // Override simulator driver file if it has been passed as command line arguments
    if (runtime_settings_->simulator_driver_path().length() > 0)
        settings_->simulator()->set_driver_file_path(runtime_settings_->simulator_driver_path());

    // Override grid file if it has been passed as command line arguments
    if (runtime_settings_->grid_file_path().length() > 0)
        settings_->model()->set_reservoir_grid_path(runtime_settings_->grid_file_path());

    // Override simulator executable path if it has been passed as command line arguments
    if (runtime_settings_->simulator_exec_script_path().length() > 0)
        settings_->simulator()->set_execution_script_path(runtime_settings_->simulator_exec_script_path());

    // Override FieldOpt build directory path if it has been passed as command line arguments
    if (runtime_settings_->fieldopt_build_dir().length() > 0)
        settings_->set_build_path(runtime_settings_->fieldopt_build_dir());
}

void AbstractRunner::InitializeModel()
{
    if (settings_ == 0)
        throw std::runtime_error("The Settings must be initialized before the Model.");

    model_ = new Model::Model(*settings_->model(), logger_);
}

void AbstractRunner::InitializeSimulator()
{
    if (model_ == 0)
        throw std::runtime_error("The Model must be initialized before the simulator.");

    switch (settings_->simulator()->type()) {
        case ::Settings::Simulator::SimulatorType::ECLIPSE:
            if (runtime_settings_->verbosity_level()) cout << "Using ECL100 reservoir simulator." << endl;
            simulator_ = new Simulation::SimulatorInterfaces::ECLSimulator(settings_, model_);
            break;
        case ::Settings::Simulator::SimulatorType::ADGPRS:
            if (runtime_settings_->verbosity_level()) cout << "Using ADGPRS reservoir simulator." << endl;
            simulator_ = new Simulation::SimulatorInterfaces::AdgprsSimulator(settings_, model_);
            break;
        case ::Settings::Simulator::SimulatorType::Flow:
            if (runtime_settings_->verbosity_level()) cout << "Using Flow reservoir simulator." << endl;
            simulator_ = new Simulation::SimulatorInterfaces::FlowSimulator(settings_, model_);
            break;
        default:
            throw std::runtime_error("Unable to initialize runner: simulator set in driver file not recognized.");
    }
    simulator_->SetVerbosityLevel(runtime_settings_->verbosity_level());
}

void AbstractRunner::EvaluateBaseModel()
{
    if (simulator_ == 0)
        throw std::runtime_error("The simulator must be initialized before evaluating the base model.");
    if (!simulator_->results()->isAvailable()) {
        if (runtime_settings_->verbosity_level()) cout << "Simulating base case." << endl;
        simulator_->Evaluate();
    }
}

void AbstractRunner::InitializeObjectiveFunction()
{
    if (simulator_ == 0 || settings_ == 0)
        throw std::runtime_error("The Simulator and the Settings must be initialized before the Objective Function.");

    switch (settings_->optimizer()->objective().type) {
        case Settings::Optimizer::ObjectiveType::WeightedSum:
            if (runtime_settings_->verbosity_level()) cout << "Using WeightedSum-type objective function." << endl;
            objective_function_ = new Optimization::Objective::WeightedSum(settings_->optimizer(), simulator_->results());
            break;
        default:
            throw std::runtime_error("Unable to initialize runner: objective function type not recognized.");
    }
}

void AbstractRunner::InitializeBaseCase()
{
    if (objective_function_ == 0 || model_ == 0)
        throw std::runtime_error("The Objective Function and the Model must be initialized before the Base Case.");
    base_case_ = new Optimization::Case(model_->variables()->GetBinaryVariableValues(),
                                        model_->variables()->GetDiscreteVariableValues(),
                                        model_->variables()->GetContinousVariableValues());
    if (!simulator_->results()->isAvailable()) {
        if (runtime_settings_->verbosity_level())
            cout << "Simulation results are unavailable. Setting base case objective function value to sentinel value." << endl;
        base_case_->set_objective_function_value(sentinelValue());
    }
    else
        base_case_->set_objective_function_value(objective_function_->value());
    if (runtime_settings_->verbosity_level()) cout << "Base case objective function value set to: " << base_case_->objective_function_value() << endl;
}

void AbstractRunner::InitializeOptimizer()
{
    if (base_case_ == 0 || model_ == 0)
        throw std::runtime_error("The Base Case and the Model must be initialized before the Optimizer");

    switch (settings_->optimizer()->type()) {
        case Settings::Optimizer::OptimizerType::Compass:
            if (runtime_settings_->verbosity_level()) cout << "Using CompassSearch optimization algorithm." << endl;
            optimizer_ = new Optimization::Optimizers::CompassSearch(settings_->optimizer(),
                                                                     base_case_,
                                                                     model_->variables(),
                                                                     model_->grid(),
                                                                     logger_
            );
            optimizer_->SetVerbosityLevel(runtime_settings_->verbosity_level());
            break;
        case Settings::Optimizer::OptimizerType::APPS:
            if (runtime_settings_->verbosity_level()) cout << "Using APPS optimization algorithm." << endl;
            optimizer_ = new Optimization::Optimizers::APPS(settings_->optimizer(),
                                                            base_case_,
                                                            model_->variables(),
                                                            model_->grid(),
                                                            logger_
            );
            optimizer_->SetVerbosityLevel(runtime_settings_->verbosity_level());
            break;
        case Settings::Optimizer::OptimizerType::GeneticAlgorithm:
            if (runtime_settings_->verbosity_level()) cout << "Using GeneticAlgorithm optimization algorithm." << endl;
            optimizer_ = new Optimization::Optimizers::RGARDD(settings_->optimizer(),
                                                              base_case_,
                                                              model_->variables(),
                                                              model_->grid(),
                                                              logger_
            );
            optimizer_->SetVerbosityLevel(runtime_settings_->verbosity_level());
            break;
        case Settings::Optimizer::OptimizerType::ExhaustiveSearch2DVert:
            if (runtime_settings_->verbosity_level()) cout << "Using ExhaustiveSearch2DVert." << endl;
            optimizer_ = new Optimization::Optimizers::ExhaustiveSearch2DVert(settings_->optimizer(),
                                                                              base_case_,
                                                                              model_->variables(),
                                                                              model_->grid(),
                                                                              logger_
            );
            optimizer_->SetVerbosityLevel(runtime_settings_->verbosity_level());
            break;
        default:
            throw std::runtime_error("Unable to initialize runner: optimization algorithm set in driver file not recognized.");
    }
    optimizer_->EnableConstraintLogging(runtime_settings_->output_dir());
}

void AbstractRunner::InitializeBookkeeper()
{
    if (settings_ == 0 || optimizer_ == 0)
        throw std::runtime_error("The Settings and the Optimizer must be initialized before the Bookkeeper.");

    bookkeeper_ = new Bookkeeper(settings_, optimizer_->case_handler());
}

void AbstractRunner::InitializeLogger(QString output_subdir, bool write_logs)
{
    logger_ = new Logger(runtime_settings_, output_subdir, write_logs);
}

void AbstractRunner::PrintCompletionMessage() const {
    cout << "Optimization complete: ";
    switch (optimizer_->IsFinished()) {
        case Optimization::Optimizer::TerminationCondition::MAX_EVALS_REACHED:
            cout << "maximum number of evaluations reached (not converged)." << endl;
            break;
        case Optimization::Optimizer::TerminationCondition::MINIMUM_STEP_LENGTH_REACHED:
            cout << "minimum step length reached (converged)." << endl;
            break;
        default: cout << "Unknown termination reason." << endl;
    }

    cout << "Best case at termination:" << optimizer_->GetTentativeBestCase()->id().toString().toStdString() << endl;
    cout << "Variable values: " << endl;
    for (auto var : optimizer_->GetTentativeBestCase()->integer_variables().keys()) {
        auto prop_name = model_->variables()->GetDiscreteVariable(var)->name();
        auto prop_val = optimizer_->GetTentativeBestCase()->integer_variables()[var];
        cout << "\t" << prop_name.toStdString() << "\t" << prop_val << endl;
    }
    for (auto var : optimizer_->GetTentativeBestCase()->real_variables().keys()) {
        auto prop_name = model_->variables()->GetContinousVariable(var)->name();
        auto prop_val = optimizer_->GetTentativeBestCase()->real_variables()[var];
        cout << "\t" << prop_name.toStdString() << "\t" << prop_val << endl;
    }
    for (auto var : optimizer_->GetTentativeBestCase()->binary_variables().keys()) {
        auto prop_name = model_->variables()->GetBinaryVariable(var)->name();
        auto prop_val = optimizer_->GetTentativeBestCase()->binary_variables()[var];
        cout << "\t" << prop_name.toStdString() << "\t" << prop_val << endl;
    }
}

int AbstractRunner::timeoutValue() const {
    if (simulation_times_.size() == 0 || runtime_settings_->simulation_timeout() == 0)
        return 10000;
    else {
        return calc_median(simulation_times_) * runtime_settings_->simulation_timeout();
    }
}

void AbstractRunner::FinalizeInitialization(bool write_logs) {
    if (write_logs) {
        logger_->AddEntry(runtime_settings_);
        logger_->FinalizePrerunSummary();
    }
}

void AbstractRunner::FinalizeRun(bool write_logs) {
    if (optimizer_ != 0) { // This indicates whether or not we're on a worker process
        model_->ApplyCase(optimizer_->GetTentativeBestCase());
        simulator_->WriteDriverFilesOnly();
        PrintCompletionMessage();
    }
    model_->Finalize();
    if (write_logs)
        logger_->FinalizePostrunSummary();
}

}
