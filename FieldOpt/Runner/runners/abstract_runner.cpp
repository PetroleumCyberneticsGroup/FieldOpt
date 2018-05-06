/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 Created: 16.12.2015 2015 by einar

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include "abstract_runner.h"

#include "Optimization/optimizers/compass_search.h"
#include "Optimization/optimizers/ExhaustiveSearch2DVert.h"
#include <Optimization/optimizers/APPS.h>
#include <Optimization/optimizers/GeneticAlgorithm.h>
#include <Optimization/optimizers/RGARDD.h>
#include <Optimization/optimizers/bayesian_optimization/EGO.h>
#include <Optimization/optimizers/DFO.h>
#include <Optimization/optimizers/SNOPTSolverC.h>
#include "Optimization/objective/weightedsum.h"

#include <Simulation/simulator_interfaces/flowsimulator.h>
#include "Simulation/simulator_interfaces/eclsimulator.h"
#include "Simulation/simulator_interfaces/adgprssimulator.h"

#include "Utilities/math.hpp"
#include "Utilities/debug.hpp"

// ---------------------------------------------------------
#include <iomanip>

// ---------------------------------------------------------
using std::cout;
using std::endl;

// ---------------------------------------------------------
namespace Runner {

// =========================================================
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

// ---------------------------------------------------------
double AbstractRunner::sentinelValue() const {

  if (settings_->optimizer()->mode() ==
      Settings::Optimizer::OptimizerMode::Minimize)
    return -1*sentinel_value_;
  return sentinel_value_;
}

// ---------------------------------------------------------
void
AbstractRunner::InitializeSettings(QString output_subdirectory) {

  // -------------------------------------------------------
  QString output_directory = runtime_settings_->output_dir();

  if (output_subdirectory.length() > 0)
    output_directory.append(QString("/%1/").arg(output_subdirectory));
  Utilities::FileHandling::CreateDirectory(output_directory);

  // -------------------------------------------------------
  settings_ = new Settings::Settings(runtime_settings_->driver_file(),
                                     output_directory,
                                     runtime_settings_->verb_vector());

  // -------------------------------------------------------
  // Override FIELDOPT BUILD DIR PATH with command line argument
  if (runtime_settings_->fieldopt_build_dir().length() > 0)
    settings_->set_build_path(runtime_settings_->fieldopt_build_dir());

  // -------------------------------------------------------
  // Override SIMULATOR DRIVER FILE with command line argument
  if (runtime_settings_->simulator_driver_path().length() > 0)
    settings_->simulator()->set_driver_file_path(
        runtime_settings_->simulator_driver_path());

  // -------------------------------------------------------
  // Override SIMULATOR INCLUDE DIR PATH with command line argument
  if (runtime_settings_->sim_incl_dir_path().length() > 0)
    settings_->simulator()->set_sim_incl_dir_path(
        runtime_settings_->sim_incl_dir_path());

  // -------------------------------------------------------
  // Override SCHEDULE DRIVER FILE with command line argument
  if (runtime_settings_->schedule_file_path().length() > 0)
    settings_->simulator()->set_schedule_file_path(
        runtime_settings_->schedule_file_path());

  // -------------------------------------------------------
  // Override GRID FILE PATH with command line argument
  if (runtime_settings_->grid_file_path().length() > 0)
    settings_->model()->set_reservoir_grid_path(
        runtime_settings_->grid_file_path());

  // -------------------------------------------------------
  // Override SIMULATOR EXEC PATH with command line argument
  if (runtime_settings_->simulator_exec_script_path().length() > 0)
    settings_->simulator()->set_execution_script_path(
        runtime_settings_->simulator_exec_script_path());

  // -------------------------------------------------------
//  settings_->optimizer()->
//      UpdateSimDirs(settings_->simulator());

  settings_->optimizer()->sim_dirs_.driver_file_path_ =
      settings_->simulator()->driver_file_path();

  settings_->optimizer()->sim_dirs_.driver_directory_=
      settings_->simulator()->driver_parent_directory();

  // -------------------------------------------------------
  if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
    cout << fstr("[run]Initialized Settings.") << endl;
}

// ---------------------------------------------------------
void AbstractRunner::InitializeModel() {

  // -------------------------------------------------------
  if (settings_ == 0)
    throw std::runtime_error(
        "The Settings must be initialized before the Model.");

  // -------------------------------------------------------
  // model_ = new Model::Model(*settings_->model(), logger_);
  model_ = new Model::Model(settings_->model(), logger_);

  if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
    std::cout << "[run]Initialized Model.------" << std::endl;
}

// ---------------------------------------------------------
void AbstractRunner::InitializeSimulator() {

  // -------------------------------------------------------
  if (model_ == 0)
    throw std::runtime_error(
        "The Model must be initialized before the simulator.");

  // -------------------------------------------------------
  switch (settings_->simulator()->type()) {

    // -----------------------------------------------------
    case ::Settings::Simulator::SimulatorType::ECLIPSE:
      if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
        cout << fstr("[run]Reservoir simulator:") << "ECL100" << endl;
      simulator_ =
          new Simulation::SimulatorInterfaces::ECLSimulator(settings_,
                                                            model_);
      break;

      // ---------------------------------------------------
    case ::Settings::Simulator::SimulatorType::ADGPRS:
      if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
        cout << fstr("[run]Reservoir simulator:") << "ADGPRS" << endl;
      simulator_ =
          new Simulation::SimulatorInterfaces::AdgprsSimulator(settings_,
                                                               model_);
      break;

      // ---------------------------------------------------
    case ::Settings::Simulator::SimulatorType::Flow:
      if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
        cout << fstr("[run]Reservoir simulator:") << "Flow" << endl;
      simulator_ =
          new Simulation::SimulatorInterfaces::FlowSimulator(settings_,
                                                             model_);
      break;

      // ---------------------------------------------------
    default:
      throw std::runtime_error(
          "Unable to initialize runner: simulator "
              "set in driver file not recognized.");
  }

  // -------------------------------------------------------
  simulator_->set_verbosity_vector(runtime_settings_->verb_vector());


  // -------------------------------------------------------
  if (settings_->verb_vector()[0] >= 1) // idx:0 -> run
    cout << fstr("[run]Initiated Simulator.") << endl;
}

// ---------------------------------------------------------
void AbstractRunner::EvaluateBaseModel() {

  // -------------------------------------------------------
  if (simulator_ == 0)
    throw std::runtime_error(
        "Simulator must be initialized before evaluating the base model.");

  // -------------------------------------------------------
  if (!simulator_->results()->isAvailable()) {
    if (settings_->verb_vector()[0] >= 1) // idx:0 -> run
      cout << fstr("[run]Simulating base case.") << endl;
    simulator_->Evaluate();
  }

  // -------------------------------------------------------
  if (settings_->verb_vector()[0] >= 1) // idx:0 -> run
    cout << fstr("[run]Evaluated BaseModel.") << endl;
}

// ---------------------------------------------------------
void AbstractRunner::InitializeObjectiveFunction() {

  // -------------------------------------------------------
  if (simulator_ == 0 || settings_ == 0)
    throw std::runtime_error(
        "Simulator & Settings must be initialized before Objective Function.");

  // -------------------------------------------------------
  switch (settings_->optimizer()->objective().type) {

    // -----------------------------------------------------
    case Settings::Optimizer::ObjectiveType::WeightedSum:
      if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
        std::cout << "[run]Objective function type: WeightedSum" << std::endl;
      objective_function_ =
          new Optimization::Objective::WeightedSum(settings_->optimizer(),
                                                   simulator_->results());
      break;

      // ---------------------------------------------------
    default:
      throw std::runtime_error(
          "Unable to initialize runner: "
              "objective function type not recognized.");
  }

  // -------------------------------------------------------
  objective_function_->
      set_verbosity_vector(runtime_settings_->verb_vector());
}

// ---------------------------------------------------------
void AbstractRunner::InitializeBaseCase() {

  // -------------------------------------------------------
  if (objective_function_ == 0 || model_ == 0)
    throw std::runtime_error(
        "Objective Function & Model must be initialized before BaseCase.");

  // -------------------------------------------------------
  base_case_ = new Optimization::Case(model_->variables());
  // Removed since all variables are passed to Case above
  // base_case_ = new Optimization::Case(model_->variables()->GetBinaryVariableValues(),
  //                                    model_->variables()->GetDiscreteVariableValues(),
  //                                    model_->variables()->GetContinuousVariableValues());


  // -------------------------------------------------------
  if (!simulator_->results()->isAvailable()) {
    if (settings_->verb_vector()[0] >= 1) { // idx:0 -> run (Runner)
      std::cout << "[run]Sim.rslts unavailable.-- "
                << "Setting BaseCase OFV set to sentinel value (="
                << fixed << setprecision(8) << sentinelValue() << ")"
                << std::endl;
    }
    base_case_->set_objective_function_value(sentinelValue());
  }
  else
    base_case_->set_objective_function_value(objective_function_->value());

  // -------------------------------------------------------
  if (settings_->verb_vector()[0] >= 1) { // idx:0 -> run (Runner)
    std::cout << "[run]Initialized BaseCase.---" << std::endl;
    std::cout << "[run]BaseCase OFV set to:---- " << fixed << setprecision(8)
              << base_case_->objective_function_value() << std::endl;
  }
}

// ---------------------------------------------------------
void AbstractRunner::InitializeOptimizer() {

  // -------------------------------------------------------
  if (base_case_ == 0 || model_ == 0)
    throw std::runtime_error(
        "BaseCase & Model must be initialized before Optimizer");

  // -------------------------------------------------------
  switch (settings_->optimizer()->type()) {

    // -----------------------------------------------------
    case Settings::Optimizer::OptimizerType::Compass:
      if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
        std::cout << "[run]Optimization algo.:----- "
                  << FRED << "CompassSearch" << END << std::endl;

      optimizer_ = new Optimization::Optimizers::CompassSearch(
          settings_->optimizer(),
          base_case_,
          model_->variables(),
          model_->grid(),
          logger_,
          model_->ricasedata());
      break;

      // ---------------------------------------------------
    case Settings::Optimizer::OptimizerType::APPS:
      if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
        std::cout << "[run]Optimization algo.:----- "
                  << FRED << "APPS" << END << std::endl;

      optimizer_ = new Optimization::Optimizers::APPS(
          settings_->optimizer(),
          base_case_,
          model_->variables(),
          model_->grid(),
          logger_);
      break;

      // ---------------------------------------------------
    case Settings::Optimizer::OptimizerType::GeneticAlgorithm:
      if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
        std::cout << "[run]Optimization algo.:----- GeneticAlgorithm" << std::endl;

      optimizer_ = new Optimization::Optimizers::RGARDD(
          settings_->optimizer(),
          base_case_,
          model_->variables(),
          model_->grid(),
          logger_);
      break;

    case Settings::Optimizer::OptimizerType::EGO:
      if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
        std::cout << "Using EGO optimization algorithm." << std::endl;

      optimizer_ = new Optimization::Optimizers::BayesianOptimization::EGO(
          settings_->optimizer(),
          base_case_,
          model_->variables(),
          model_->grid(),
          logger_
      );

      break;

      // ---------------------------------------------------
    case Settings::Optimizer::OptimizerType::ExhaustiveSearch2DVert:
      if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
        std::cout << "[run]Optimization algo.:----- ExhaustiveSearch2DVert" << std::endl;

      optimizer_ = new Optimization::Optimizers::ExhaustiveSearch2DVert(
          settings_->optimizer(),
          base_case_,
          model_->variables(),
          model_->grid(),
          logger_);
      break;

      // ---------------------------------------------------
    case Settings::Optimizer::OptimizerType::SNOPTSolver:
      if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
        std::cout << "[run]Optimization algo.:----- SNOPTSolverC" << std::endl;

      optimizer_ = new Optimization::Optimizers::SNOPTSolverC(
          settings_->optimizer(),
          base_case_,
          model_->variables(),
          model_->grid(),
          logger_);
      break;

      // ---------------------------------------------------
    case Settings::Optimizer::OptimizerType::DFO:
      if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
        std::cout << "[run]Optimization algo.:----- DFO" << std::endl;

      optimizer_ = new Optimization::Optimizers::DFO(
          settings_->optimizer(),
          base_case_,
          model_->variables(),
          model_->grid(),
          logger_);
      break;

      // ---------------------------------------------------
    default:
      throw std::runtime_error(
          "Unable to initialize runner: optimization algorithm "
              "set in driver file not recognized.");
  }

  // -------------------------------------------------------
  optimizer_->EnableConstraintLogging(runtime_settings_->output_dir());
  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Initialized Optimizer.--" << endl;
}

// ---------------------------------------------------------
void AbstractRunner::InitializeBookkeeper() {

  if (settings_ == 0 || optimizer_ == 0)
    throw std::runtime_error(
        "The Settings and the Optimizer must "
            "be initialized before the Bookkeeper.");

  bookkeeper_ = new Bookkeeper(settings_, optimizer_->case_handler());
  if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
    std::cout << "[run]Initialized Bookkeeper.-" << std::endl;
}

// ---------------------------------------------------------
void AbstractRunner::InitializeLogger(QString output_subdir,
                                      bool write_logs) {

  logger_ = new Logger(runtime_settings_, output_subdir, write_logs);
  if (settings_->verb_vector()[0] >= 1) // idx:0 -> run (Runner)
    std::cout << "[run]Initialized Logger.-----" << std::endl;
}

// ---------------------------------------------------------
void AbstractRunner::PrintCompletionMessage() const {

  std::cout << "[run]Optimization complete:- ";
  // -------------------------------------------------------
  switch (optimizer_->IsFinished()) {
    case Optimization::Optimizer::TerminationCondition::MAX_EVALS_REACHED:
      std::cout << "maximum number of evaluations reached (not converged)." << std::endl;
      break;
    case Optimization::Optimizer::TerminationCondition::MINIMUM_STEP_LENGTH_REACHED:
      std::cout << "minimum step length reached (converged)." << std::endl;
      break;
    default: std::cout << "Unknown termination reason." << std::endl;
  }

  // -------------------------------------------------------
  std::cout << "[run]Best.case @ opt.end:--- "
            << optimizer_->GetTentativeBestCase()->id().toString().toStdString() << std::endl;
  std::cout << "[run]Variable values:------- " << std::endl;

  // -------------------------------------------------------
  for (auto var : optimizer_->GetTentativeBestCase()->integer_variables().keys()) {
    auto prop_name = model_->variables()->GetDiscreteVariable(var)->name();
    auto prop_val = optimizer_->GetTentativeBestCase()->integer_variables()[var];
    std::cout << "\t" << prop_name.toStdString() << "\t" << prop_val << std::endl;
  }

  // -------------------------------------------------------
  for (auto var : optimizer_->GetTentativeBestCase()->real_variables().keys()) {
    auto prop_name = model_->variables()->GetContinousVariable(var)->name();
    auto prop_val = optimizer_->GetTentativeBestCase()->real_variables()[var];
    std::cout << "\t" << prop_name.toStdString() << "\t" << prop_val << std::endl;
  }

  // -------------------------------------------------------
  for (auto var : optimizer_->GetTentativeBestCase()->binary_variables().keys()) {
    auto prop_name = model_->variables()->GetBinaryVariable(var)->name();
    auto prop_val = optimizer_->GetTentativeBestCase()->binary_variables()[var];
    std::cout << "\t" << prop_name.toStdString() << "\t" << prop_val << std::endl;
  }
}

// ---------------------------------------------------------
int AbstractRunner::timeoutValue() const {

  // -------------------------------------------------------
  if (simulation_times_.size() == 0 || runtime_settings_->simulation_timeout() == 0)
    return 10000;
  else {
    return calc_median(simulation_times_) * runtime_settings_->simulation_timeout();
  }
}

// ---------------------------------------------------------
void AbstractRunner::FinalizeInitialization(bool write_logs) {

  // -------------------------------------------------------
  if (write_logs) {
    logger_->AddEntry(runtime_settings_);
    logger_->FinalizePrerunSummary();
  }
}

// ---------------------------------------------------------
void AbstractRunner::FinalizeRun(bool write_logs) {

  // -------------------------------------------------------
  if (optimizer_ != 0) { // This indicates whether or not we're on a worker process
    model_->ApplyCase(optimizer_->GetTentativeBestCase());
    simulator_->WriteDriverFilesOnly();
    PrintCompletionMessage();
  }

  // -------------------------------------------------------
  model_->Finalize();
  if (write_logs)
    logger_->FinalizePostrunSummary();
}

}
