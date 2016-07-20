#ifndef FIELDOPT_OVERSEER_H
#define FIELDOPT_OVERSEER_H

#include "mpi_runner.h"

namespace Runner {
    namespace MPI {
        /*!
         * @brief The Overseer class takes care of distributing cases between workers. The runner taken as an
         * is primarily used for the common MPI helpers.
         */
        class Overseer {
        public:

            Overseer(MPIRunner *runner);

        private:
            MPIRunner *runner_;
        };
    }
}


#endif //FIELDOPT_OVERSEER_H
