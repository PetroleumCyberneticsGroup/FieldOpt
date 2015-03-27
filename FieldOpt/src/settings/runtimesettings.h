#ifndef RUNTIMESETTINGS_H
#define RUNTIMESETTINGS_H

#include <QString>
#include "optimizersettings.h"
#include "simulatorsettings.h"

class OptimizerSettings;
class SimulatorSettings;

/*!
 * \brief Holds settings used during run time by some runner program.
 *
 * An object of this class is included in a Driver object.
 *
 * These settings are set by the DriverReader class.
 */
class RuntimeSettings
{
private:
    bool parallel;                //!< Whether or not to run in parallel.
    int parallelRuns;             //!< The number of parallel runs to be made.
    QString debugFilename;        //!< Path to the debug file to be written.
    OptimizerSettings* optimizerSettings;  //!< General settings for the optimizer.
    SimulatorSettings* simulatorSettings;  //!< General settings for the simulator.

public:
    RuntimeSettings();            //!< Initializes the fields to NOT_SET.

    QString getDebugFilename() const;
    void setDebugFilename(const QString &value);

    int getParallelRuns() const;
    void setParallelRuns(int value);

    bool getParallel() const;
    void setParallel(bool value);

    QString toString() const;

    OptimizerSettings *getOptimizerSettings() const;
    void setOptimizerSettings(OptimizerSettings *value);

    SimulatorSettings *getSimulatorSettings() const;
    void setSimulatorSettings(SimulatorSettings *value);
};

#endif // RUNTIMESETTINGS_H
