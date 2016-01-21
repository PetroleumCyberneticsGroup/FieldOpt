#ifndef SERIALRUNNER_H
#define SERIALRUNNER_H

#include "abstract_runner.h"

namespace Runner {

class MainRunner;

/*!
 * \brief The SerialRunner class is a very simple implementation of a serial runner.
 * It interacts with the optimizer and starts simulations in series (i.e. it is _very_ slow).
 */
class SerialRunner : public AbstractRunner
{
    friend class MainRunner;
private:
    SerialRunner(RuntimeSettings *runtime_settings);

    // AbstractRunner interface
private:
    void Execute();
};

}

#endif // SERIALRUNNER_H
