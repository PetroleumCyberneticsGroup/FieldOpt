#ifndef COMPASSSEARCHOPTIMIZER_H
#define COMPASSSEARCHOPTIMIZER_H

#include "gssalgorithm.h"
#include "optimizer.h"
#include "constraints/integerboundaryconstraint.h"
#include "constraints/doubleboundaryconstraint.h"
#include "bookkeeper.h"

class CompassSearchOptimizer : public Optimizer, public GSSAlgorithm
{
private:
    QVector<Case*> perturb(Case *c);
    bool isBetter(Case *c);
    Bookkeeper* bookkeeper;

public:
    CompassSearchOptimizer(){ evals = 0; }

    void initialize(Case* baseCase, OptimizerSettings* settings);
    QVector<Case*> getNewCases();
    void compareCases(QVector<Case*> cases);
    bool isFinished();

    void reduceStepLength();

    QString getStatusString();
};

#endif // COMPASSSEARCHOPTIMIZER_H
