#include "synchronous_mpi_runner.h"

namespace Runner {
    namespace MPI {

        SynchronousMPIRunner::SynchronousMPIRunner(RuntimeSettings *rts) : MPIRunner(rts) {
            assert(world_.size() >= 2 && "The SynchronousMPIRunner requires at least two MPI processes.");

            if (world_.rank() == 0) {
                std::cout << "00" << std::endl;
                InitializeSettings("rank" + QString::number(rank()));

                InitializeModel();
                InitializeSimulator();
                EvaluateBaseModel();
                InitializeObjectiveFunction();
                InitializeBaseCase();
                InitializeOptimizer();
                InitializeBookkeeper();
                InitializeLogger();
                overseer_ = new MPI::Overseer(this);
            }
            else {
                InitializeSettings("rank" + QString::number(rank()));
                InitializeModel();
                InitializeSimulator();
                worker_ = new MPI::Worker(this);
            }
        }

        void SynchronousMPIRunner::Execute() {
            if (rank() == 0) {
                initialDistribution();
                while (optimizer_->IsFinished() == false) {
                    // Get an evaluated case
                    // Submit the case to the optimizer
                    // Get new case
                    // Send termination signal to workers if done
                }
            }
            else {
                worker_->RecvUnevaluatedCase();
                while (worker_->GetCurrentCase() != nullptr) {
                    // Evaluate the case
                    // Return the case
                    // Recv new case
                }
            }
        }

        void SynchronousMPIRunner::initialDistribution() {
            while (optimizer_->nr_queued_cases() > 0 && overseer_->NumberOfFreeWorkers() > 0) {
                overseer_->AssignCase(optimizer_->GetCaseForEvaluation());
            }
        }
    }
}
