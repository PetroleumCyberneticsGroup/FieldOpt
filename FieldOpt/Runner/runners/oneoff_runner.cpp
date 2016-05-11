#include "oneoff_runner.h"
#include <iostream>

namespace Runner {

OneOffRunner::OneOffRunner(RuntimeSettings *runtime_settings)
    : AbstractRunner(runtime_settings)
{
    std::cout << "Initializing one-off runner." << std::endl;
}

void OneOffRunner::Execute()
{
    std::cout << "Executing one-off runner." << std::endl;
}

}
