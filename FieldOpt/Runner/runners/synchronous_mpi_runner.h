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
         *
         * Still todo:
         *   - Probably some more logging and console debug messages
         *   - Utilizing bookkeeping
         *   - Timeout on simulations
         *   - Handle cases that cant validly be applied to model
         *   - Logging of simulation, production data and compdat and runner stats
         */
        class SynchronousMPIRunner : public MPIRunner {
        public:
            SynchronousMPIRunner(RuntimeSettings *rts);

            virtual void Execute();

        private:
            MPI::Overseer *overseer_;
            MPI::Worker *worker_;

            /*!
             * @brief Distribute cases to be evaluated to all but one worker.
             */
            void initialDistribution();

            void printMessage(std::string message);
        };

    }
}

#endif //FIELDOPT_SYNCHRONOUS_MPI_RUNNER_H
