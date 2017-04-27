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

typedef Optimization::Optimizer::TerminationCondition TermCondition;
typedef Optimization::Case::CaseState::EvalStatus EvalStatus;
typedef Optimization::Case::CaseState::ErrorMessage ErrorMessage;

using namespace std;

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
    while (optimizer_->IsFinished() == TermCondition::NOT_FINISHED) {
        auto new_case = optimizer_->GetCaseForEvaluation();

        if (bookkeeper_->IsEvaluated(new_case, true)) {
            new_case->state.eval = EvalStatus::E_BOOKKEEPED;
        }
        else {
            try {
                bool simulation_success = true;
                new_case->state.eval = EvalStatus::E_CURRENT;
                model_->ApplyCase(new_case);

                auto start = QDateTime::currentDateTime();
                // Wny this condition: simulation_times_.size() == 0?
                if (simulation_times_.size() == 0 ||
                    runtime_settings_->simulation_timeout() == 0) {
                    simulator_->Evaluate();
                }
                else {
                    simulation_success =
                        simulator_->Evaluate(
                            timeoutValue(),
                            runtime_settings_->threads_per_sim()
                        );
                }
                auto end = QDateTime::currentDateTime();
                int sim_time = time_span_seconds(start, end);

                if (simulation_success) {
                    new_case->set_objective_function_value(
                        objective_function_->value()
                    );
                    new_case->state.eval = EvalStatus::E_DONE;
                    new_case->SetSimTime(sim_time);
                    simulation_times_.push_back((sim_time));
                }
                else {
                    new_case->set_objective_function_value(sentinelValue());
                    new_case->state.eval = EvalStatus::E_FAILED;
                    new_case->state.err_msg = ErrorMessage::ERR_SIM;
                    if (sim_time >= timeoutValue())
                        new_case->state.eval = EvalStatus::E_TIMEOUT;
                }
            } catch (runtime_error e) {
                cout << e.what() << endl;
                // Obsolete message?:
                cout << "Invalid well block coordinate encountered. "
                    "Setting obj. val. to sentinel value." << endl;

                new_case->set_objective_function_value(sentinelValue());
                new_case->state.eval = EvalStatus::E_FAILED;
                new_case->state.err_msg = ErrorMessage::ERR_WIC;
            }
        }
        optimizer_->SubmitEvaluatedCase(new_case);
    }
    FinalizeRun(true);
}

}
