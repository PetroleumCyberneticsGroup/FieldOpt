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

SelectedOptimizer RuntimeSettings::getOptimizer() const
{
    return optimizer;
}

void RuntimeSettings::setOptimizer(const SelectedOptimizer &value)
{
    optimizer = value;
}

SelectedSimulator RuntimeSettings::getSimulator() const
{
    return simulator;
}

void RuntimeSettings::setSimulator(const SelectedSimulator &value)
{
    simulator = value;
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
RuntimeSettings::RuntimeSettings()
{
    simulator = SIM_NOT_SET;
    optimizer = OPT_NOT_SET;
    debugFilename = "NOT_SET";
    parallel = true;
    parallelRuns = 2;
}
