#ifndef ONEOFF_H
#define ONEOFF_H

#include "abstract_runner.h"

namespace Runner {

class MainRunner;

/*!
 * \brief The OneOff class represents a "one-off" runner. Its purpose is to
 * construct the base case, apply constraints to it, simulate it, and print
 * the results to an easily readable file.
 *
 * Note that, currently, this only supports models with two wells: one producer
 * named PRODUCER and one injector named INJECTOR.
 */
class OneOffRunner : AbstractRunner
{
    friend class MainRunner;
public:
    OneOffRunner(RuntimeSettings *runtime_settings);

    // AbstractRunner interface
private:
    void Execute();
};

}

#endif // ONEOFF_H
