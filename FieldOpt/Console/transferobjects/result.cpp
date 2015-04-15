#include "result.h"

Result::Result(Case* c, int id) :
    perturbation_id(id),
    result(c->objectiveValue())
{}
