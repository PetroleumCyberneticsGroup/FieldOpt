#ifndef RESULT_H
#define RESULT_H

#include "optimizers/case.h"

class Result
{
private:
    const int perturbation_id;
    const double result;

public:
    Result(Case* c, int id);

    int getPerturbation_id() const { return perturbation_id; }
    double getResult() const { return result; }
};

#endif // RESULT_H
