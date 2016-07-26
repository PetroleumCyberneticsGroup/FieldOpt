#ifndef FIELDOPT_WOKER_H
#define FIELDOPT_WOKER_H

#include "mpi_runner.h"

namespace Runner {
    namespace MPI {
        /*!
         * @brief The Worker class is responsible for receiving and sending from/to an overseer object.
         * The runner taken as a parameter in the constructor is primarily used for the common MPI helpers.
         */
        class Worker {
        public:
            Worker(MPIRunner *runner);

            /*!
             * @brief Receive an unevaluated case from the Scheduler and set it as the current_case_.
             */
            void RecvUnevaluatedCase();

            /*!
             * @brief Send the current_case_ back to the Scheduler.
             */
            void SendEvaluatedCase();

            Optimization::Case *GetCurrentCase();

        private:
            MPIRunner *runner_;
            Optimization::Case *current_case_;
        };
    }
}

#endif //FIELDOPT_WOKER_H
