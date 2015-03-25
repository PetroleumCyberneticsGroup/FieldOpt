#ifndef RUNTIMESETTINGS_H
#define RUNTIMESETTINGS_H

#include <QString>
#include "optimizersettings.h"

enum SelectedSimulator {SIM_NOT_SET, MRST, GPRS, VLP};
enum SelectedOptimizer {OPT_NOT_SET, RUNONCE, LSH, EROPT};

class OptimizerSettings;

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
    SelectedSimulator simulator;  //!< The simulator specified in the driver file.
    SelectedOptimizer optimizer;  //!< The Optimizer specified in the driver file.
    QString debugFilename;        //!< Path to the debug file to be written.
    OptimizerSettings optimizerSettings;  //!< General settings for the optimizer.

public:
    RuntimeSettings();            //!< Initializes the fields to NOT_SET.

    OptimizerSettings getOptimizerSettings() const;
    void setOptimizerSettings(const OptimizerSettings &value);

    QString getDebugFilename() const;
    void setDebugFilename(const QString &value);

    SelectedOptimizer getOptimizer() const;
    void setOptimizer(const SelectedOptimizer &value);

    SelectedSimulator getSimulator() const;
    void setSimulator(const SelectedSimulator &value);

    int getParallelRuns() const;
    void setParallelRuns(int value);

    bool getParallel() const;
    void setParallel(bool value);
};

#endif // RUNTIMESETTINGS_H
