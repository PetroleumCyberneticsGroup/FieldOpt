#ifndef FIELDOPT_WOKER_H
#define FIELDOPT_WOKER_H

#include "mpi_runner.h"

namespace Runner {
    namespace MPI {
        /*!
         * @brief The Worker class is responsible for executing simulations passed to it by an Overseer object.
         * The runner taken as a parameter in the constructor is primarily used for the common MPI helpers.
         */
        class Worker {
        public:
            Worker(MPIRunner *runner);

        private:
            MPIRunner *runner_;
        };
    }
}

#endif //FIELDOPT_WOKER_H
