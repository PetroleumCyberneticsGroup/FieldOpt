#include "compasssearchoptimizer.h"



QVector<Case *> CompassSearchOptimizer::perturb(Case *c)
{
    emitProgress(QString("Perturbin cases.\n%1").arg(getStatusString()));
    QVector<Case*> newCases;
    // Binary variables
    for (int i = 0; i < c->numberOfBinaryVariables(); ++i) {
        Case* newCasePositive = new Case(*c);
        newCasePositive->setBinaryVariableValue(i, newCasePositive->binaryVariableValue(i) + step_length);
        if (newCasePositive->boundariesOk())
            newCases.append(newCasePositive);

        Case* newCaseNegative= new Case(*c);
        newCaseNegative->setBinaryVariableValue(i, newCaseNegative->binaryVariableValue(i) - step_length);
        if (newCaseNegative->boundariesOk())
            newCases.append(newCaseNegative);
    }
    emitProgress("Perturbed binary vars.");

    // Integer variables
    for (int i = 0; i < c->numberOfIntegerVariables(); ++i) {
        Case* newCasePositive = new Case(*c);
        newCasePositive->setIntegerVariableValue(i, newCasePositive->integerVariableValue(i) + step_length);
        if (newCasePositive->boundariesOk())
            newCases.append(newCasePositive);

        Case* newCaseNegative= new Case(*c);
        newCaseNegative->setIntegerVariableValue(i, newCaseNegative->integerVariableValue(i) - step_length);
        if (newCaseNegative->boundariesOk())
            newCases.append(newCaseNegative);
    }
    emitProgress("Perturbed integer vars.");

    // Real variables
    for (int i = 0; i < c->numberOfRealVariables(); ++i) {
        Case* newCasePositive = new Case(*c);
        newCasePositive->setRealVariableValue(i, newCasePositive->realVariableValue(i) + step_length);
        if (newCasePositive->boundariesOk())
            newCases.append(newCasePositive);

        Case* newCaseNegative= new Case(*c);
        newCaseNegative->setRealVariableValue(i, newCaseNegative->realVariableValue(i) - step_length);
        if (newCaseNegative->boundariesOk())
            newCases.append(newCaseNegative);
    }
    emitProgress("Perturbed real vars.");
    return newCases;
}

bool CompassSearchOptimizer::isBetter(Case *c)
{
    emitProgress(QString("Checking isBetter.\n%1").arg(getStatusString()));
    if (c->objectiveValue() < best_case->objectiveValue())
        return true;
    else
        return false;
}


void CompassSearchOptimizer::initialize(Case *baseCase, OptimizerSettings* settings)
{
    best_case  = baseCase;
    minimum_step_length = settings->getMinimumStepLength();
    step_length = settings->getInitialStepLength();
}

QVector<Case *> CompassSearchOptimizer::getNewCases()
{
    emitProgress(QString("Generating new cases.\n%1").arg(getStatusString()));
    new_cases.clear();
    new_cases = perturb(best_case);
    evals += new_cases.size();
    emitProgress("Returning new cases.");
    return new_cases;
}

void CompassSearchOptimizer::compareCases(QVector<Case *> cases)
{
    emitProgress(QString("Comparing cases.\n%1").arg(getStatusString()));
    bool foundBetter = false;
    for (int i = 0; i < cases.length(); ++i) {
        if (isBetter(cases.at(i))) {
            delete best_case;
            best_case = cases.at(i);
            foundBetter = true;
        }
    }
    if (!foundBetter) {
        reduceStepLength();
    }
}

bool CompassSearchOptimizer::isFinished()
{
    emitProgress(QString("Checking isFinished.\n%1").arg(getStatusString()));
    if (evals > max_evals || step_length < minimum_step_length)
        return true;
    else
        return false;
}

void CompassSearchOptimizer::reduceStepLength()
{
    emitProgress(QString("Reducing step length.\n%1").arg(getStatusString()));
    step_length = 0.5 * step_length;
}

QString CompassSearchOptimizer::getStatusString() {
    return QString("Evals: %1\nStep length: %2").arg(evals).arg(step_length);
}
