#include "runtimesettings.h"



OptimizerSettings RuntimeSettings::getOptimizerSettings() const
{
    return optimizerSettings;
}

void RuntimeSettings::setOptimizerSettings(const OptimizerSettings &value)
{
    optimizerSettings = value;
}

QString RuntimeSettings::getDebugFilename() const
{
    return debugFilename;
}

void RuntimeSettings::setDebugFilename(const QString &value)
{
    debugFilename = value;
}

int RuntimeSettings::getParallelRuns() const
{
    return parallelRuns;
}

void RuntimeSettings::setParallelRuns(int value)
{
    parallelRuns = value;
}

bool RuntimeSettings::getParallel() const
{
    return parallel;
}

void RuntimeSettings::setParallel(bool value)
{
    parallel = value;
}

SimulatorSettings RuntimeSettings::getSimulatorSettings() const
{
    return simulatorSettings;
}

void RuntimeSettings::setSimulatorSettings(const SimulatorSettings &value)
{
    simulatorSettings = value;
}
RuntimeSettings::RuntimeSettings()
{
    simulatorSettings = SimulatorSettings();
    optimizerSettings = OptimizerSettings();
    debugFilename = "NOT_SET";
    parallel = true;
    parallelRuns = 2;
}
