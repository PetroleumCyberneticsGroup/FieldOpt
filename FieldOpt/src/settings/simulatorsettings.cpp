#include "simulatorsettings.h"


SelectedSimulator SimulatorSettings::getSimulator() const
{
    return simulator;
}

void SimulatorSettings::setSimulator(const SelectedSimulator &value)
{
    simulator = value;
}

QString SimulatorSettings::toString() const
{
    QString simstr = "Simulator settings:\n";
    simstr.append("Selected simulator: ");
    switch (simulator) {
    case SIM_NOT_SET:
        simstr.append("Not set\n");
        break;
    case MRST:
        simstr.append("MRST\n");
        break;
    case GPRS:
        simstr.append("GPRS\n");
        break;
    case VLP:
        simstr.append("VLP\n");
    default:
        break;
    }
    return simstr;
}
SimulatorSettings::SimulatorSettings()
{
    simulator = SIM_NOT_SET;
}
