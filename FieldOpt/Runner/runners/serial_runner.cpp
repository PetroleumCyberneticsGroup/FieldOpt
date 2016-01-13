#include "serial_runner.h"
#include "Optimization/objective/weightedsum.h"

namespace Runner {

SerialRunner::SerialRunner(Runner::RuntimeSettings *runtime_settings)
    : AbstractRunner(runtime_settings)
{}

void SerialRunner::Execute()
{
    while (optimizer_->IsFinished() == false) {
        auto new_case = optimizer_->GetCaseForEvaluation();
        if (runtime_settings_->verbose()) std::cout << "Got new case from optimizer." << std::endl;
        model_->ApplyCase(new_case);
        if (runtime_settings_->verbose()) std::cout << "Evaluating new case..." << std::endl;
        simulator_->Evaluate();
        if (runtime_settings_->verbose()) std::cout << "New case evaluated." << std::endl;
        new_case->set_objective_function_value(objective_function_->value());
        if (runtime_settings_->verbose()) std::cout << "Set new case objective function value: " << new_case->objective_function_value() << std::endl;
        optimizer_->SubmitEvaluatedCase(new_case);
        if (runtime_settings_->verbose()) std::cout << "New case submitted to optimizer." << std::endl;
    }
}

}
