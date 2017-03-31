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
        std::cout << "00" << std::endl;
        InitializeSettings("rank" + QString::number(rank()));

        InitializeModel();
        InitializeSimulator();
        EvaluateBaseModel();
        InitializeObjectiveFunction();
        InitializeBaseCase();
        InitializeOptimizer();
        InitializeBookkeeper();
        InitializeLogger();
        overseer_ = new MPI::Overseer(this);
    }
    else {
        InitializeSettings("rank" + QString::number(rank()));
        InitializeModel();
        InitializeSimulator();
        InitializeObjectiveFunction();
        InitializeLogger("rank" + QString::number(rank()));
        worker_ = new MPI::Worker(this);
    }
}

void SynchronousMPIRunner::Execute() {
    auto handle_new_case = [&]() mutable {
      printMessage("Getting new case from optimizer.", 2);
      auto new_case = optimizer_->GetCaseForEvaluation();
      if (bookkeeper_->IsEvaluated(new_case, true)) {
          printMessage("Case found in bookkeeper");
          new_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_BOOKKEEPED;
          optimizer_->SubmitEvaluatedCase(new_case);
      }
      else {
          overseer_->AssignCase(new_case);
          printMessage("New case assigned to worker.", 2);
      }
    };
    auto wait_for_evaluated_case = [&]() mutable {
      printMessage("Waiting to receive evaluated case...", 2);
      auto evaluated_case = overseer_->RecvEvaluatedCase(); // TODO: This is a duplicate case that wont get deleted, i.e. a MEMORY LEAK.
      printMessage("Evaluated case received.", 2);
      switch (overseer_->last_case_tag) {
          case MPIRunner::MsgTag::CASE_EVAL_SUCCESS:
              evaluated_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_DONE;
              if (optimizer_->GetSimulationDuration(evaluated_case) > 0){
                  simulation_times_.push_back(optimizer_->GetSimulationDuration(evaluated_case));
              }
              break;
          case MPIRunner::MsgTag::CASE_EVAL_INVALID:
              evaluated_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_FAILED;
              evaluated_case->state.err_msg = Optimization::Case::CaseState::ErrorMessage::ERR_UNKNOWN;
              break;
          case MPIRunner::MsgTag::CASE_EVAL_TIMEOUT:
              evaluated_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_TIMEOUT;
              break;
      }
      optimizer_->SubmitEvaluatedCase(evaluated_case);
      printMessage("Submitted evaluated case to optimizer.", 2);
    };
    if (rank() == 0) {
        printMessage("Performing initial distribution...", 2);
        initialDistribution();
        printMessage("Initial distribution done.", 2);
        while (optimizer_->IsFinished() == false) {
            if (optimizer_->nr_queued_cases() > 0) { // Queued cases in optimizer
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
        printMessage("Terminating workers.", 2);
        PrintCompletionMessage();
        overseer_->TerminateWorkers();
    }
    else {
        printMessage("Waiting to receive initial unevaluated case...", 2);
        worker_->RecvUnevaluatedCase();
        printMessage("Reveived initial unevaluated case.", 2);
        while (worker_->GetCurrentCase() != nullptr) {
            MPIRunner::MsgTag tag = MPIRunner::MsgTag::CASE_EVAL_SUCCESS; // Tag to be sent along with the case.
            try {
                bool simulation_success = true;
                printMessage("Applying case to model.", 2);
                model_->ApplyCase(worker_->GetCurrentCase());
                if (simulation_times_.size() == 0 || runtime_settings_->simulation_timeout() == 0) {
                    printMessage("Starting model evaluation.", 2);
                    simulator_->Evaluate();
                }
                else {
                    printMessage("Starting model evaluation with timeout.", 2);
                    simulation_success = simulator_->Evaluate(timeoutValue(), runtime_settings_->threads_per_sim());
                }
                if (simulation_success) {
                    tag = MPIRunner::MsgTag::CASE_EVAL_SUCCESS;
                    printMessage("Setting objective function value.", 2);
                    worker_->GetCurrentCase()->set_objective_function_value(objective_function_->value());
                }
                else {
                    tag = MPIRunner::MsgTag::CASE_EVAL_TIMEOUT;
                    printMessage("Timed out. Setting objective function value to SENTINEL VALUE.", 2);
                    worker_->GetCurrentCase()->set_objective_function_value(sentinelValue());
                }
            } catch (std::runtime_error e) {
                std::cout << e.what() << std::endl;
                tag = MPIRunner::MsgTag::CASE_EVAL_INVALID;
                printMessage("Invalid well block coordinate encountered. Setting obj. val. to sentinel value.");
                printMessage("Invalid case. Setting objective function value to SENTINEL VALUE.", 2);
                worker_->GetCurrentCase()->set_objective_function_value(sentinelValue());
            }
            printMessage("Sending back evaluated case.", 2);
            worker_->SendEvaluatedCase(tag);
            printMessage("Waiting to reveive an unevaluated case...", 2);
            worker_->RecvUnevaluatedCase();
            printMessage("Received an unevaluated case.", 2);
        }
    }
}

void SynchronousMPIRunner::initialDistribution() {
    while (optimizer_->nr_queued_cases() > 0 && overseer_->NumberOfFreeWorkers() > 1) { // Leave one free worker
        overseer_->AssignCase(optimizer_->GetCaseForEvaluation());
    }
}
}
}
