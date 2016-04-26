#include "serial_runner.h"
#include "Optimization/objective/weightedsum.h"
#include "Runner/logger.h"
#include "Model/wells/well_exceptions.h"

namespace Runner {

SerialRunner::SerialRunner(Runner::RuntimeSettings *runtime_settings)
    : AbstractRunner(runtime_settings)
{}

void SerialRunner::Execute()
{
    auto logger = Logger(runtime_settings_);
    logger.LogSettings(settings_);
    logger.WritePropertyUuidNameMap(model_);
    logger.LogCase(base_case_);
    while (optimizer_->IsFinished() == false) {
        auto new_case = optimizer_->GetCaseForEvaluation();
        logger.LogCase(new_case);

        if (bookkeeper_->IsEvaluated(new_case, true)) {
            logger.LogCase(new_case, "Case objective value set by bookkeeper.");
        }
        else {
            try {
                model_->ApplyCase(new_case);
                logger.LogSimulation(new_case); // Log sim start
                simulator_->Evaluate();
                logger.LogSimulation(new_case); // Log sim end
                new_case->set_objective_function_value(objective_function_->value());
            } catch (std::runtime_error e) {
                std::cout << e.what() << std::endl;
                std::cout << "Invalid well block coordinate encountered. Setting obj. val. to sentinel value." << std::endl;
                setObjectiveFunctionSentinelValue(new_case);
            }
            logger.LogCase(new_case);
        }
        optimizer_->SubmitEvaluatedCase(new_case);
        logger.LogOptimizerStatus(optimizer_);
    }
}

}
