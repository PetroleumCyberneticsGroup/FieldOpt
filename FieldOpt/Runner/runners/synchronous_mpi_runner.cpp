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
                logger_->LogSettings(settings_);
                logger_->WritePropertyUuidNameMap(model_);
                logger_->LogCase(base_case_);
                if (runtime_settings_->verbosity_level() >= 2) printMessage("Performing initial distribution...");
                initialDistribution();
                if (runtime_settings_->verbosity_level() >= 2) printMessage("Initial distribution done.");
                while (optimizer_->IsFinished() == false) {
                    if (!(optimizer_->nr_queued_cases() == 0 && overseer_->NumberOfBusyWorkers() > 0)) {
                        if (runtime_settings_->verbosity_level() >= 2) printMessage("Assigning new case to worker...");
                        auto new_case = optimizer_->GetCaseForEvaluation();
                        logger_->LogCase(new_case);
                        overseer_->AssignCase(new_case);
                        if (runtime_settings_->verbosity_level() >= 2) printMessage("New case assigned to worker.");
                    } else printMessage("Waiting for all evaluations in iteration to complete...");
                    if (runtime_settings_->verbosity_level() >= 2) printMessage("Waiting to receive evaluated case...");
                    auto evaluated_case = overseer_->RecvEvaluatedCase(); // TODO: This is a duplicate case that wont get deleted, i.e. a MEMORY LEAK.
                    logger_->LogCase(evaluated_case);
                    if (runtime_settings_->verbosity_level() >= 2) printMessage("Evaluated case received.");
                    optimizer_->SubmitEvaluatedCase(evaluated_case);
                    if (runtime_settings_->verbosity_level() >= 2) printMessage("Submitted evaluated case to optimizer.");
                    logger_->LogOptimizerStatus(optimizer_);
                }
                if (runtime_settings_->verbosity_level() >= 2) printMessage("Terminating workers.");
                PrintCompletionMessage();
                overseer_->TerminateWorkers();
            }
            else {
                if (runtime_settings_->verbosity_level() >= 2) printMessage("Waiting to receive initial unevaluated case...");
                worker_->RecvUnevaluatedCase();
                if (runtime_settings_->verbosity_level() >= 2) printMessage("Reveived initial unevaluated case.");
                while (worker_->GetCurrentCase() != nullptr) {
                    if (runtime_settings_->verbosity_level() >= 2) printMessage("Applying case to model.");
                    model_->ApplyCase(worker_->GetCurrentCase());
                    if (runtime_settings_->verbosity_level() >= 2) printMessage("Starting model evaluation.");
                    simulator_->Evaluate();
                    if (runtime_settings_->verbosity_level() >= 2) printMessage("Setting objective function value.");
                    worker_->GetCurrentCase()->set_objective_function_value(objective_function_->value());
                    if (runtime_settings_->verbosity_level() >= 2) printMessage("Sending back evaluated case.");
                    worker_->SendEvaluatedCase();
                    if (runtime_settings_->verbosity_level() >= 2) printMessage("Waiting to reveive an unevaluated case...");
                    worker_->RecvUnevaluatedCase();
                    if (runtime_settings_->verbosity_level() >= 2) printMessage("Received an unevaluated case.");
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
