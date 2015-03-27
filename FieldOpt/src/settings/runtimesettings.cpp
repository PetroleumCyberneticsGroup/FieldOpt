#include "runtimesettings.h"

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

QString RuntimeSettings::toString() const
{
    QString rtsstr = "Runtime settings:\n";
    rtsstr.append("Run in parallel: " + QString::number(parallel) + "\n");
    rtsstr.append("Number of parallel runs: " + QString::number(parallelRuns) + "\n");
    rtsstr.append("Debug filename: " + debugFilename + "\n");
    rtsstr.append(optimizerSettings->toString());
    rtsstr.append(simulatorSettings->toString());
    return rtsstr;
}


OptimizerSettings *RuntimeSettings::getOptimizerSettings() const
{
    return optimizerSettings;
}

void RuntimeSettings::setOptimizerSettings(OptimizerSettings *value)
{
    optimizerSettings = value;
}

SimulatorSettings *RuntimeSettings::getSimulatorSettings() const
{
    return simulatorSettings;
}

void RuntimeSettings::setSimulatorSettings(SimulatorSettings *value)
{
    simulatorSettings = value;
}
RuntimeSettings::RuntimeSettings()
{
    simulatorSettings = new SimulatorSettings();
    optimizerSettings = new OptimizerSettings();
    debugFilename = "NOT_SET";
    parallel = true;
    parallelRuns = 2;
}
