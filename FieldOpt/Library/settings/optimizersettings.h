#ifndef OPTIMIZERSETTINGS_H
#define OPTIMIZERSETTINGS_H

#include <QList>
#include <QString>

enum SelectedOptimizer {OPT_NOT_SET, RUNONCE, LSH, EROPT, COMPASS};

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
    int maxEvaluations;
    double initialStepLength;
    double minimumStepLength;
    double perturbationSize;
    double termination;
    double bookkeeperTolerance;
    int terminationStart;
    bool startingpointUpdate;
    QList<int> steps;  //!< Steps to be used. Only used by the EROPT optimizer. \todo Implement EROPT optimizer.
    SelectedOptimizer optimizer;  //!< The Optimizer specified in the driver file.


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

    SelectedOptimizer getOptimizer() const;
    void setOptimizer(const SelectedOptimizer &value);

    QString toString() const;
    double getInitialStepLength() const;
    void setInitialStepLength(double value);
    double getMinimumStepLength() const;
    void setMinimumStepLength(double value);
    double getBookkeeperTolerance() const;
    void setBookkeeperTolerance(double value);
    int getMaxEvaluations() const;
    void setMaxEvaluations(int value);
};

#endif // OPTIMIZERSETTINGS_H
