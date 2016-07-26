#ifndef FIELDOPT_OVERSEER_H
#define FIELDOPT_OVERSEER_H

#include "mpi_runner.h"
#include "Utilities/time.hpp"

namespace Runner {
    namespace MPI {
        /*!
         * @brief The Overseer class takes care of distributing cases between workers. The runner taken as an
         * is primarily used for the common MPI helpers.
         */
        class Overseer {
        public:

            Overseer(MPIRunner *runner);

            /*!
             * @brief Assign a Case to a Worker. If no workers are free, the scheduler will wait untill one is.
             * @param c The case to be assigned to a Worker for evaluation.
             */
            void AssignCase(Optimization::Case *c);

            /*!
             * @brief Wait to receive an evaluated case.
             * @return An evaluated case object.
             */
            Optimization::Case *RecvEvaluatedCase();

            /*!
             * @brief Get the number of workers that are currently not performing any work.
             */
            int NumberOfFreeWorkers();

            /*!
             * @brief The WorkerStatus struct holds information about the current status of all workers in the network.
             */
            struct WorkerStatus {
                WorkerStatus() { rank = -1; }
                WorkerStatus(int r) { rank = r;}
                int rank; //!< The rank of the process the worker is running on.
                bool working = false; //!< Indicates if the worker is currently performing simulations.
                QDateTime working_since; //!< The last time a job was sent to the worker.
                int working_seconds() { //!< Number of seconds since last work was sent to the process.
                    return time_since_seconds(working_since);
                }
                /*!
                 * @brief Start the worker. Should be called whenever work is sent to the worker. This marks is as
                 * working.
                 */
                void start() {
                    working = true;
                    working_since = QDateTime::currentDateTime();
                }
                /*!
                 * @brief Stop the worker. This should be called whenever results are received from the worker. This
                 * marks the worker as not working.
                 */
                void stop() {
                    working = false;
                }
            };

            /*!
                 * @brief Get the status for the longest running worker.
                 * @return A copy of the status object for the longest running worker.
                 */
            WorkerStatus GetLongestRunningWorker();

        private:
            MPIRunner *runner_;
            QHash<int, WorkerStatus> workers_; //!< A map of the workers. The key is the rank of the process.

            WorkerStatus & getFreeWorker(); //!< Get a worker not marked as working.
        };
    }
}


#endif //FIELDOPT_OVERSEER_H
