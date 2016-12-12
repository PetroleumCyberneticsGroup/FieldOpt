#include "serial_runner.h"

namespace Runner {

SerialRunner::SerialRunner(Runner::RuntimeSettings *runtime_settings)
    : AbstractRunner(runtime_settings)
{
    InitializeSettings();
    InitializeModel();
    InitializeSimulator();
    EvaluateBaseModel();
    InitializeObjectiveFunction();
    InitializeBaseCase();
    InitializeOptimizer();
    InitializeBookkeeper();
    InitializeLogger();
}

void SerialRunner::Execute()
{
    logger_->LogSettings(settings_);
    logger_->WritePropertyUuidNameMap(model_);
    logger_->LogCase(base_case_);
    logger_->LogCompdat(base_case_, simulator_->GetCompdatString());
    logger_->LogProductionData(base_case_, simulator_->results());
    while (optimizer_->IsFinished() == Optimization::Optimizer::TerminationCondition::NOT_FINISHED) {
        auto new_case = optimizer_->GetCaseForEvaluation();

        if (bookkeeper_->IsEvaluated(new_case, true)) {
            logger_->LogCase(new_case, "Case objective value set by bookkeeper.");
            logger_->increment_bookkeeped_cases();
        }
        else {
            try {
                bool simulation_success = true;
                model_->ApplyCase(new_case);
                logger_->LogSimulation(new_case); // Log sim start
                logger_->LogCompdat(new_case, simulator_->GetCompdatString()); // Log compdat
                if (logger_->shortest_simulation_time() == 0 || runtime_settings_->simulation_timeout() == 0) {
                    simulator_->Evaluate();
                }
                else {
                    simulation_success = simulator_->Evaluate(logger_->shortest_simulation_time() * runtime_settings_->simulation_timeout());
                }
                if (simulation_success) {
                    logger_->LogSimulation(new_case);
                    new_case->set_objective_function_value(objective_function_->value());
                    logger_->LogProductionData(new_case, simulator_->results());
                }
                else {
                    logger_->LogSimulation(new_case, false);
                    new_case->set_objective_function_value(sentinelValue());
                }
                logger_->increment_simulated_cases();
            } catch (std::runtime_error e) {
                std::cout << e.what() << std::endl;
                std::cout << "Invalid well block coordinate encountered. Setting obj. val. to sentinel value." << std::endl;
                logger_->increment_invalid_cases();
                new_case->set_objective_function_value(sentinelValue());
            }
            logger_->LogCase(new_case);
        }
        optimizer_->SubmitEvaluatedCase(new_case);
        logger_->LogOptimizerStatus(optimizer_);
        logger_->LogRunnerStats();
    }
    PrintCompletionMessage();
}

}
