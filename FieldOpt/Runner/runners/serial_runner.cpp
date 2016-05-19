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
    while (optimizer_->IsFinished() == false) {
        auto new_case = optimizer_->GetCaseForEvaluation();
        logger_->LogCase(new_case);

        if (bookkeeper_->IsEvaluated(new_case, true)) {
            logger_->LogCase(new_case, "Case objective value set by bookkeeper.");
        }
        else {
            try {
                model_->ApplyCase(new_case);
                logger_->LogSimulation(new_case); // Log sim start
                logger_->LogCompdat(new_case, simulator_->GetCompdatString()); // Log compdat
                simulator_->Evaluate();
                logger_->LogSimulation(new_case); // Log sim end
                logger_->LogProductionData(new_case, simulator_->results());
                new_case->set_objective_function_value(objective_function_->value());
            } catch (std::runtime_error e) {
                std::cout << e.what() << std::endl;
                std::cout << "Invalid well block coordinate encountered. Setting obj. val. to sentinel value." << std::endl;
                new_case->set_objective_function_value(sentinelValue());
            }
            logger_->LogCase(new_case);
        }
        optimizer_->SubmitEvaluatedCase(new_case);
        logger_->LogOptimizerStatus(optimizer_);
    }
}

}
