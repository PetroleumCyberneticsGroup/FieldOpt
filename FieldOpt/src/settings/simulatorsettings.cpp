#include "simulatorsettings.h"


SelectedSimulator SimulatorSettings::getSimulator() const
{
    return simulator;
}

void SimulatorSettings::setSimulator(const SelectedSimulator &value)
{
    simulator = value;
}
SimulatorSettings::SimulatorSettings()
{
    simulator = SIM_NOT_SET;
}
