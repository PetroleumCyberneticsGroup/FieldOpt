#ifndef OPTIMIZERSETTINGS_H
#define OPTIMIZERSETTINGS_H

#include <QList>

/*!
 * \brief Class containing general optimizer settings to be used during runtime.
 *
 * \todo Use this class to initialize an Optimizer object.
 */
class OptimizerSettings
{
private:
    int maxIterations;
    int maxIterContinuous;
    double perturbationSize;
    double termination;
    int terminationStart;
    bool startingpointUpdate;
    QList<int> steps;  //!< Steps to be used. Only used by the EROPT optimizer. \todo Implement EROPT optimizer.


public:
    OptimizerSettings();

    int getMaxIterations() const;
    void setMaxIterations(int value);

    int getMaxIterContinuous() const;
    void setMaxIterContinuous(int value);

    double getPerturbationSize() const;
    void setPerturbationSize(double value);

    double getTermination() const;
    void setTermination(double value);

    int getTerminationStart() const;
    void setTerminationStart(int value);

    bool getStartingpointUpdate() const;
    void setStartingpointUpdate(bool value);

    QList<int> getSteps() const;
    void setSteps(const QList<int> &value);
};

#endif // OPTIMIZERSETTINGS_H
