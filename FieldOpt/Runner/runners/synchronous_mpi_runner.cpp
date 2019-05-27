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
#include "synchronous_mpi_runner.h"

namespace Runner {
namespace MPI {

SynchronousMPIRunner::SynchronousMPIRunner(RuntimeSettings *rts) : MPIRunner(rts) {
    assert(world_.size() >= 2 && "The SynchronousMPIRunner requires at least two MPI processes.");

    if (world_.rank() == 0) {
        InitializeSettings("rank" + QString::number(rank()));

        InitializeLogger();
        InitializeModel();
        InitializeSimulator();
        EvaluateBaseModel();
        InitializeObjectiveFunction();
        InitializeBaseCase();
        InitializeOptimizer();
        InitializeBookkeeper();
        overseer_ = new MPI::Overseer(this);
        FinalizeInitialization(true);
    }
    else {
        InitializeLogger("rank" + QString::number(rank()));
        InitializeSettings("rank" + QString::number(rank()));
        InitializeModel();
        InitializeSimulator();
        InitializeObjectiveFunction();
        worker_ = new MPI::Worker(this);
        FinalizeInitialization(false);
    }
}

void SynchronousMPIRunner::Execute() {

    auto handle_new_case = [&]() mutable {
      Optimization::Case *new_case;
      if (ensemble_helper_.IsCaseAvailableForEval()) {
          printMessage("Getting new case from ensemble helper.", 2);
          new_case = ensemble_helper_.GetCaseForEval();
      }
      else {
          printMessage("Getting new case from optimizer.", 2);
          new_case = optimizer_->GetCaseForEvaluation();
          if (is_ensemble_run_) {
              ensemble_helper_.SetActiveCase(new_case);
              new_case = ensemble_helper_.GetCaseForEval();
          }
      }
      if (!is_ensemble_run_ && bookkeeper_->IsEvaluated(new_case, true)) {
          printMessage("Case found in bookkeeper");
          new_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_BOOKKEEPED;
          optimizer_->SubmitEvaluatedCase(new_case);
      }
      else {
          if (is_ensemble_run_) {
              int worker_rank = ensemble_helper_.GetAssignedWorker(new_case->GetEnsembleRealization().toStdString(), overseer_->GetFreeWorkerRanks());
              overseer_->AssignCase(new_case, worker_rank);
          }
          else {
              overseer_->AssignCase(new_case);
          }
          printMessage("New case assigned to worker.", 2);
      }
    };

    auto wait_for_evaluated_case = [&]() mutable {
      printMessage("Waiting to receive evaluated case...", 2);
      auto evaluated_case = overseer_->RecvEvaluatedCase(); // TODO: This is a duplicate case that wont get deleted, i.e. a MEMORY LEAK.
      printMessage("Evaluated case received.", 2);
      if (overseer_->last_case_tag == MPIRunner::MsgTag::CASE_EVAL_SUCCESS) {
          printMessage("Setting state for evaluated case.", 2);
          evaluated_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_DONE;
          printMessage("Setting timings for evaluated case.", 2);
          if (!is_ensemble_run_ && optimizer_->GetSimulationDuration(evaluated_case) > 0){
              printMessage("Setting timings for evaluated case.", 2);
              simulation_times_.push_back(optimizer_->GetSimulationDuration(evaluated_case));
          }
      }
      if (is_ensemble_run_) {
          printMessage("Submitting evaluated realization to ensemble helper.", 2);
          ensemble_helper_.SubmitEvaluatedRealization(evaluated_case);
          if (ensemble_helper_.IsCaseDone()) {
              printMessage("All selected realizations evaluated. Getting composite case.", 2);
              auto evaluated_case = ensemble_helper_.GetEvaluatedCase();
              evaluated_case->set_objective_function_value(evaluated_case->GetEnsembleAverageOfv());
              optimizer_->SubmitEvaluatedCase(evaluated_case);
              model_->ApplyCase(evaluated_case);
              printMessage("Submitted evaluated case to optimizer and model.", 2);
          }
      }
      else {
          optimizer_->SubmitEvaluatedCase(evaluated_case);
          printMessage("Submitted evaluated case to optimizer.", 2);
      }
    };

    if (rank() == 0) { // Overseer
        printMessage("Performing initial distribution...", 2);
        initialDistribution();
        printMessage("Initial distribution done.", 2);
        while (optimizer_->IsFinished() == false) {
            if (is_ensemble_run_) {
                printMessage(ensemble_helper_.GetStateString(), 2);
            }
            if (is_ensemble_run_ && ensemble_helper_.IsCaseAvailableForEval()) {
                printMessage("Queued realization cases available.", 2);
                if (overseer_->NumberOfFreeWorkers() > 0) { // Free workers available
                    printMessage("Free workers available. Handling next case.", 2);
                    handle_new_case();
                }
                else { // No workers available
                    printMessage("No free workers available. Waiting for an evaluated case.", 2);
                    wait_for_evaluated_case();
                }
            }
            else if (is_ensemble_run_ && !ensemble_helper_.IsCaseDone()) {
                printMessage("Not all ensemble realizations have been evaluated.", 2);
                wait_for_evaluated_case();
            }
            else if (optimizer_->nr_queued_cases() > 0) { // Queued cases in optimizer
                printMessage("Queued cases available.", 2);
                if (overseer_->NumberOfFreeWorkers() > 0) { // Free workers available
                    printMessage("Free workers available. Handling next case.", 2);
                    handle_new_case();
                }
                else { // No workers available
                    printMessage("No free workers available. Waiting for an evaluated case.", 2);
                    wait_for_evaluated_case();
                }
            }
            else { // No queued cases in optimizer
                printMessage("No queued cases available.", 2);
                if (overseer_->NumberOfBusyWorkers() == 0) { // All workers are free
                    printMessage("No workers are busy. Starting next iteration.", 2);
                    handle_new_case();
                }
                else { // Some workers are performing simulations
                    printMessage("Some workers are still evaluating cases from this iteration. Waiting for evaluated cases.", 2);
                    wait_for_evaluated_case();
                }
            }
        }
        FinalizeRun(true);
        overseer_->TerminateWorkers();
        printMessage("Terminating workers.", 2);
        overseer_->EnsureWorkerTermination();
        env_.~environment();
        return;
    }

    else { // Worker
        printMessage("Waiting to receive initial unevaluated case...", 2);
        worker_->RecvUnevaluatedCase();
        printMessage("Reveived initial unevaluated case.", 2);
        while (worker_->GetCurrentCase() != nullptr) {
            MPIRunner::MsgTag tag = MPIRunner::MsgTag::CASE_EVAL_SUCCESS; // Tag to be sent along with the case.
            try {
                model_update_done_ = false;
                simulation_done_ = false;
                logger_->AddEntry(this);
                bool simulation_success = true;
                if (is_ensemble_run_) {
                    printMessage("Updating grid path.", 2);
                    model_->set_grid_path(ensemble_helper_.GetRealization(worker_->GetCurrentCase()->GetEnsembleRealization().toStdString()).grid());
                }
                printMessage("Applying case to model.", 2);
                model_->ApplyCase(worker_->GetCurrentCase());
                model_update_done_ = true; logger_->AddEntry(this);
                auto start = QDateTime::currentDateTime();
                if (runtime_settings_->simulation_timeout() == 0 && settings_->simulator()->max_minutes() < 0) {
                    printMessage("Starting model evaluation.", 2);
                    simulator_->Evaluate();
                }
                else if (simulation_times_.size() == 0 && settings_->simulator()->max_minutes() > 0) {
                    if (!is_ensemble_run_) {
                        printMessage("Starting model evaluation with timeout.", 2);
                        simulation_success = simulator_->Evaluate(settings_->simulator()->max_minutes() * 60,
                                                                  runtime_settings_->threads_per_sim());
                    }
                    else {
                        printMessage("Starting ensemble model evaluation with timeout.", 2);
                        simulation_success = simulator_->Evaluate(ensemble_helper_.GetRealization(worker_->GetCurrentCase()->GetEnsembleRealization().toStdString()),
                                                                  settings_->simulator()->max_minutes() * 60,
                                                                  runtime_settings_->threads_per_sim());
                    }
                }
                else {
                    if (!is_ensemble_run_) {
                        printMessage("Starting model evaluation with timeout.", 2);
                        simulation_success = simulator_->Evaluate(timeoutValue(), runtime_settings_->threads_per_sim());
                    }
                    else {
                        printMessage("Starting ensemble model evaluation with timeout.", 2);
                        simulation_success = simulator_->Evaluate(ensemble_helper_.GetRealization(worker_->GetCurrentCase()->GetEnsembleRealization().toStdString()),
                                                                  settings_->simulator()->max_minutes() * 60,
                                                                  runtime_settings_->threads_per_sim());
                    }
                }
                simulation_done_ = true; logger_->AddEntry(this);
                auto end = QDateTime::currentDateTime();
                int sim_time = time_span_seconds(start, end);
                if (simulation_success) {
                    tag = MPIRunner::MsgTag::CASE_EVAL_SUCCESS;
                    printMessage("Setting objective function value.", 2);
                    model_->wellCost(settings_->optimizer());
                    worker_->GetCurrentCase()->set_objective_function_value(objective_function_->value());
                    worker_->GetCurrentCase()->SetSimTime(sim_time);
                    worker_->GetCurrentCase()->state.eval = Optimization::Case::CaseState::EvalStatus::E_DONE;
                    simulation_times_.push_back(sim_time);
                }
                else {
                    tag = MPIRunner::MsgTag::CASE_EVAL_TIMEOUT;
                    printMessage("Timed out. Setting objective function value to SENTINEL VALUE.", 2);
                    worker_->GetCurrentCase()->state.eval = Optimization::Case::CaseState::EvalStatus::E_TIMEOUT;
                    worker_->GetCurrentCase()->state.err_msg = Optimization::Case::CaseState::ErrorMessage::ERR_SIM;
                    worker_->GetCurrentCase()->set_objective_function_value(sentinelValue());
                }
            } catch (std::runtime_error e) {
                std::cout << e.what() << std::endl;
                tag = MPIRunner::MsgTag::CASE_EVAL_INVALID;
                worker_->GetCurrentCase()->state.eval = Optimization::Case::CaseState::EvalStatus::E_FAILED;
                worker_->GetCurrentCase()->state.err_msg = Optimization::Case::CaseState::ErrorMessage::ERR_WIC;
                printMessage("Invalid case. Setting objective function value to SENTINEL VALUE.", 2);
                worker_->GetCurrentCase()->set_objective_function_value(sentinelValue());
            }
            printMessage("Sending back evaluated case.", 2);
            worker_->SendEvaluatedCase(tag);
            printMessage("Waiting to reveive an unevaluated case...", 2);
            worker_->RecvUnevaluatedCase();
            if (worker_->GetCurrentTag() == TERMINATE) {
                printMessage("Received termination message. Breaking.", 2);
                break;
            }
            else {
                printMessage("Received an unevaluated case.", 2);
            }
        }
        FinalizeRun(false);
        printMessage("Finalized on worker.", 2);
        worker_->ConfirmFinalization();
        env_.~environment();
        return;
    }
}

void SynchronousMPIRunner::initialDistribution() {

    if (!is_ensemble_run_) { // Single-realization run
        while (optimizer_->nr_queued_cases() > 0 && overseer_->NumberOfFreeWorkers() > 1) { // Leave one free worker
            overseer_->AssignCase(optimizer_->GetCaseForEvaluation());
        }
    }
    else { // Ensemble run
        auto next_case = optimizer_->GetCaseForEvaluation();
        ensemble_helper_.SetActiveCase(next_case);
        while (ensemble_helper_.IsCaseAvailableForEval() && overseer_->NumberOfFreeWorkers() > 1) {
            overseer_->AssignCase(ensemble_helper_.GetCaseForEval());
        }

    }
}
Loggable::LogTarget SynchronousMPIRunner::GetLogTarget() {
    return STATE_RUNNER;
}
map<string, string> SynchronousMPIRunner::GetState() {
    map<string, string> statemap;
    statemap["case-desc"] = worker_->GetCurrentCase()->StringRepresentation(model_->variables());
    statemap["mod-update-done"] = model_update_done_ ? "yes" : "no";
    statemap["sim-done"] = simulation_done_ ? "yes" : "no";
    statemap["last-update"] = timestamp_string();
    return statemap;
}
QUuid SynchronousMPIRunner::GetId() {
    return QUuid();
}
}
}
