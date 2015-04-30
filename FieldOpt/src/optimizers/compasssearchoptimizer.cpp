#include "compasssearchoptimizer.h"



QVector<Case *> CompassSearchOptimizer::perturb(Case *c)
{
    QVector<Case*> newCases;
    // Binary variables
    for (int i = 0; i < c->numberOfBinaryVariables(); ++i) {
        Case* newCasePositive = new Case(*c);
        newCasePositive->setBinaryVariableValue(i, newCasePositive->binaryVariableValue(i) + step_length);
        if (isValid(newCasePositive)) {
            newCases.append(newCasePositive);
            bookkeeper->addEntry(newCasePositive);
        }

        Case* newCaseNegative= new Case(*c);
        newCaseNegative->setBinaryVariableValue(i, newCaseNegative->binaryVariableValue(i) - step_length);
        if (isValid(newCaseNegative)) {
            bookkeeper->addEntry(newCaseNegative);
            newCases.append(newCaseNegative);
        }
    }

    // Integer variables
    for (int i = 0; i < c->numberOfIntegerVariables(); ++i) {
        Case* newCasePositive = new Case(*c);
        newCasePositive->setIntegerVariableValue(i, newCasePositive->integerVariableValue(i) + step_length);
        if (isValid(newCasePositive)) {
            newCases.append(newCasePositive);
            bookkeeper->addEntry(newCasePositive);
        }

        Case* newCaseNegative= new Case(*c);
        newCaseNegative->setIntegerVariableValue(i, newCaseNegative->integerVariableValue(i) - step_length);
        if (isValid(newCaseNegative)) {
            bookkeeper->addEntry(newCaseNegative);
            newCases.append(newCaseNegative);
        }
    }

    // Real variables
    for (int i = 0; i < c->numberOfRealVariables(); ++i) {
        Case* newCasePositive = new Case(*c);
        newCasePositive->setRealVariableValue(i, newCasePositive->realVariableValue(i) + step_length);
        if (isValid(newCasePositive)) {
            newCases.append(newCasePositive);
            bookkeeper->addEntry(newCasePositive);
        }

        Case* newCaseNegative= new Case(*c);
        newCaseNegative->setRealVariableValue(i, newCaseNegative->realVariableValue(i) - step_length);
        if (isValid(newCaseNegative)) {
            bookkeeper->addEntry(newCaseNegative);
            newCases.append(newCaseNegative);
        }
    }
    return newCases;
}

bool CompassSearchOptimizer::isBetter(Case *c)
{
    if (c->objectiveValue() < best_case->objectiveValue())
        return true;
    else
        return false;
}

bool CompassSearchOptimizer::isValid(Case *c)
{
    if (c->boundariesOk() == true) {  // Valid if the box conditions are not violated ...
        if (bookkeeper->isCalculated(c) == false || bookkeeper->getTolerance() > step_length) {  // and either the case has not been calculated OR the current step length is less than the bookkeeper tolerance
            return true;
        }
    }
    return false;
}


void CompassSearchOptimizer::initialize(Case *baseCase, OptimizerSettings* settings)
{
    best_case  = baseCase;
    minimum_step_length = settings->getMinimumStepLength();
    step_length = settings->getInitialStepLength();
    bookkeeper = new Bookkeeper(settings->getBookkeeperTolerance());
}

QVector<Case *> CompassSearchOptimizer::getNewCases()
{
    new_cases.clear();
    new_cases = perturb(best_case);
    evals += new_cases.size();
    return new_cases;
}

void CompassSearchOptimizer::compareCases(QVector<Case *> cases)
{
    bool foundBetter = false;
    for (int i = 0; i < cases.size(); ++i) {
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
    if (evals > max_evals || step_length < minimum_step_length)
        return true;
    else
        return false;
}

void CompassSearchOptimizer::reduceStepLength()
{
    step_length = 0.5 * step_length;
}

QString CompassSearchOptimizer::getStatusString() {
    return QString("Evals: %1\nStep length: %2").arg(evals).arg(step_length);
}
