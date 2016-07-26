#ifndef FIELDOPT_SYNCHRONOUS_MPI_RUNNER_H
#define FIELDOPT_SYNCHRONOUS_MPI_RUNNER_H

#include "mpi_runner.h"
#include "overseer.h"
#include "worker.h"

namespace Runner {
    namespace MPI {

        /*!
         * @brief The SynchronousMPIRunner class performs the optimization synchronously in parallel. Depending
         * on the process rank, it will instantiate either an Overseer (rank = 0) to handle optimizer iteraction
         * and logging, or a Worker (rank > 0) to execute simulations.
         */
        class SynchronousMPIRunner : public MPIRunner {
        public:
            SynchronousMPIRunner(RuntimeSettings *rts);

            virtual void Execute();

        private:
            MPI::Overseer *overseer_;
            MPI::Worker *worker_;

            void initialDistribution();
        };

    }
}

#endif //FIELDOPT_SYNCHRONOUS_MPI_RUNNER_H
