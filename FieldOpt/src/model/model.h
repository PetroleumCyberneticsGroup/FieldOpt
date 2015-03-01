#ifndef MODEL_H
#define MODEL_H

#include "model/components/well.h"
#include "model/reservoir.h"

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
