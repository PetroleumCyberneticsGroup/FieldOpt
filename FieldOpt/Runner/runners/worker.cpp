#include "worker.h"

namespace Runner {
    namespace MPI {

        Worker::Worker(MPIRunner *runner) {
            runner_ = runner;
            runner_->RecvModelSynchronizationObject();
            std::cout << "Initialized Worker on " << runner_->world().rank() << std::endl;
        }

        void Worker::RecvUnevaluatedCase() {
            delete current_case_;
            current_case_ = runner_->RecvCase(runner_->scheduler_rank_, MPIRunner::MsgTag::CASE_UNEVAL);
        }

        void Worker::SendEvaluatedCase() {
            runner_->SendCase(current_case_, runner_->scheduler_rank_, MPIRunner::MsgTag::CASE_EVAL);
        }

        Optimization::Case *Worker::GetCurrentCase() {
            return current_case_;
        }

    }
}
