#ifndef SERIALRUNNER_H
#define SERIALRUNNER_H

#include "abstract_runner.h"

namespace Runner {

class MainRunner;

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
