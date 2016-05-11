#include "oneoff_runner.h"
#include <iostream>

namespace Runner {

OneOffRunner::OneOffRunner(RuntimeSettings *runtime_settings)
    : AbstractRunner(runtime_settings)
{
    std::cout << "Initializing one-off runner." << std::endl;
    InitializeSettings();
    InitializeModel();
    InitializeSimulator();
    //EvaluateBaseModel();
    InitializeObjectiveFunction();
    InitializeBaseCase();
    //InitializeOptimizer();
    //InitializeBookkeeper();
    InitializeLogger();
    applyWellPositionFromArguments();
}

void OneOffRunner::Execute()
{
    std::cout << "Executing one-off runner." << std::endl;
}

void OneOffRunner::applyWellPositionFromArguments()
{
    QList<QPair<QUuid, QString>> prod_vars = model_->variables()->GetContinousVariableNamesAndIdsMatchingSubstring("PRODUCER");
    QList<QPair<QUuid, QString>> inje_vars = model_->variables()->GetContinousVariableNamesAndIdsMatchingSubstring("INJECTOR");
    std::cout << prod_vars.length() << ", " << inje_vars.length() << std::endl;
    if (prod_vars.length() != 6 || inje_vars.length() != 6)
        throw std::runtime_error("Found an incorrect number of variables for the wells.");
}

}
