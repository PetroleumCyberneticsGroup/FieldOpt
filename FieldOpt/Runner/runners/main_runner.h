#ifndef RUNNER_H
#define RUNNER_H

#include <stdexcept>
#include "runtime_settings.h"
#include "abstract_runner.h"

namespace Runner {

/*!
 * \brief The MainRunner class takes care of initializing and starting the actual runner indicated in the runtime settings.
 */
    class MainRunner
    {
    public:
        MainRunner(RuntimeSettings *rts);

        /*!
         * \brief Execute Start the optimization run by calling the Execute function in the simulator.
         */
        void Execute();

    private:
        RuntimeSettings *runtime_settings_;
        AbstractRunner *runner_;
    };

}

#endif // RUNNER_H
