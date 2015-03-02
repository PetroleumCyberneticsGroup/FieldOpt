#ifndef MODEL_H
#define MODEL_H

#include "components/well.h"
#include "reservoir.h"
#include "objectives/objective.h"
#include "components/pipe.h"

class Well;
class Reservoir;
class Objective;
class Pipe;
class Capacity;
class IntVariable;
class RealVariable;
class BinaryVariable;
class Constraint;
class Component;
class UserConstraint;
class Cost;
class Logger;


/*!
 * \brief Abstract base class for models.
 *
 * The Model stores all the sub-parts of the problem, e.g. reseroiv, wells, pipes, separators, etc.
 */
class Model
{
public:
    Model();
    ~Model();
};

#endif // MODEL_H
