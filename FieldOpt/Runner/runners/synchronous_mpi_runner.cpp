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
                InitializeObjectiveFunction();
                worker_ = new MPI::Worker(this);
            }
        }

        void SynchronousMPIRunner::Execute() {
            if (rank() == 0) {
                initialDistribution();
                while (optimizer_->IsFinished() == false) {
                    overseer_->AssignCase(optimizer_->GetCaseForEvaluation());
                    auto evaluated_case = overseer_->RecvEvaluatedCase(); // TODO: This is a duplicate case that wont get deleted, i.e. a MEMORY LEAK.
                    optimizer_->SubmitEvaluatedCase(evaluated_case);
                }
                overseer_->TerminateWorkers();
            }
            else {
                worker_->RecvUnevaluatedCase();
                while (worker_->GetCurrentCase() != nullptr) {
                    model_->ApplyCase(worker_->GetCurrentCase());
                    simulator_->Evaluate();
                    worker_->GetCurrentCase()->set_objective_function_value(objective_function_->value());
                    worker_->SendEvaluatedCase();
                    worker_->RecvUnevaluatedCase();
                }
            }
        }

        void SynchronousMPIRunner::initialDistribution() {
            while (optimizer_->nr_queued_cases() > 0 && overseer_->NumberOfFreeWorkers() > 1) { // Leave one free worker
                overseer_->AssignCase(optimizer_->GetCaseForEvaluation());
            }
        }

        void SynchronousMPIRunner::printMessage(std::string message) {
            std::cout << "RANK " << world_.rank() << ": " << message << std::endl;
        }
    }
}
