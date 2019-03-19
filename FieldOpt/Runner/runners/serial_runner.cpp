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
#include <Utilities/time.hpp>
#include "serial_runner.h"
#include "Utilities/printer.hpp"
#include "Model/model.h"

namespace Runner {

SerialRunner::SerialRunner(Runner::RuntimeSettings *runtime_settings)
    : AbstractRunner(runtime_settings)
{
    InitializeLogger();
    InitializeSettings();
    InitializeModel();
    InitializeSimulator();
    EvaluateBaseModel();
    InitializeObjectiveFunction();
    InitializeBaseCase();
    InitializeOptimizer();
    InitializeBookkeeper();
    FinalizeInitialization(true);
}

void SerialRunner::Execute()
{
    while (optimizer_->IsFinished() == Optimization::Optimizer::TerminationCondition::NOT_FINISHED) {
        Optimization::Case *new_case;
        if (is_ensemble_run_) {
            if (ensemble_helper_.IsCaseDone()) {
                ensemble_helper_.SetActiveCase(optimizer_->GetCaseForEvaluation());
            }
            if (VERB_RUN >= 3) Printer::ext_info("Getting ensemble case.", "Runner", "Serial Runner");
            new_case = ensemble_helper_.GetCaseForEval();
            model_->set_grid_path(ensemble_helper_.GetRealization(new_case->GetEnsembleRealization().toStdString()).grid());
        }
        else {
            if (VERB_RUN >= 3) Printer::ext_info("Getting case from Optimizer.", "Runner", "Serial Runner");
            new_case = optimizer_->GetCaseForEvaluation();
            if (VERB_RUN >= 3) Printer::ext_info("Got case from Optimizer.", "Runner", "Serial Runner");
        }

        if (!is_ensemble_run_ && bookkeeper_->IsEvaluated(new_case, true)) {
            if (VERB_RUN >= 3) Printer::ext_info("Bookkeeped case.", "Runner", "Serial Runner");
            new_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_BOOKKEEPED;
        }
        else {
            try {
                bool simulation_success = true;
                new_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_CURRENT;
                if (VERB_RUN >= 3) Printer::ext_info("Applying case to model.", "Runner", "Serial Runner");
                model_->ApplyCase(new_case);
                auto start = QDateTime::currentDateTime();
                if (!is_ensemble_run_ && (simulation_times_.size() == 0 || runtime_settings_->simulation_timeout() == 0)) {
                    if (VERB_RUN >= 3) Printer::ext_info("Simulating case.", "Runner", "Serial Runner");
                    simulator_->Evaluate();
                }
                else {
                    if (is_ensemble_run_) {
                        if (VERB_RUN >= 3) Printer::ext_info("Simulating ensemble case.", "Runner", "Serial Runner");
                        simulation_success = simulator_->Evaluate(
                            ensemble_helper_.GetRealization(new_case->GetEnsembleRealization().toStdString()),
                            timeoutValue(),
                            runtime_settings_->threads_per_sim()
                        );
                    }
                    else {
                        if (VERB_RUN >= 3) Printer::ext_info("Simulating case.", "Runner", "Serial Runner");
                        simulation_success = simulator_->Evaluate(
                            timeoutValue(),
                            runtime_settings_->threads_per_sim()
                        );
                    }
                }
                if (VERB_RUN >= 3) Printer::ext_info("Done simulating case.", "Runner", "Serial Runner");
                auto end = QDateTime::currentDateTime();
                int sim_time = time_span_seconds(start, end);
                if (simulation_success) {
                    model_->wellCost(settings_->optimizer());
                    new_case->set_objective_function_value(objective_function_->value());
                    new_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_DONE;
                    new_case->SetSimTime(sim_time);
                    simulation_times_.push_back((sim_time));
                }
                else {
                    new_case->set_objective_function_value(sentinelValue());
                    new_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_FAILED;
                    new_case->state.err_msg = Optimization::Case::CaseState::ErrorMessage::ERR_SIM;
                    if (sim_time >= timeoutValue())
                        new_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_TIMEOUT;
                }
            } catch (std::runtime_error e) {
                Printer::ext_warn("Exception thrown while applying/simulating case: " + std::string(e.what()) + ". Setting obj. fun. value to sentinel value.", "Runner", "SerialRunner");
                new_case->set_objective_function_value(sentinelValue());
                new_case->state.eval = Optimization::Case::CaseState::EvalStatus::E_FAILED;
                new_case->state.err_msg = Optimization::Case::CaseState::ErrorMessage::ERR_WIC;
            }
        }
        if (is_ensemble_run_) {
            ensemble_helper_.SubmitEvaluatedRealization(new_case);
            if  (ensemble_helper_.IsCaseDone()) {
                optimizer_->SubmitEvaluatedCase(ensemble_helper_.GetEvaluatedCase());
            }
        }
        else {
            if (VERB_RUN >= 3) Printer::ext_info("Submitting evaluated case to Optimizer.", "Runner", "Serial Runner");
            optimizer_->SubmitEvaluatedCase(new_case);
        }
    }
    FinalizeRun(true);
}

}
