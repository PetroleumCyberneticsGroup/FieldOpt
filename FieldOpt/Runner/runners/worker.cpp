#include "worker.h"

namespace Runner {
    namespace MPI {

        Worker::Worker(MPIRunner *runner) {
            runner_ = runner;
            runner_->RecvModelSynchronizationObject();
            std::cout << "Initialized Worker on " << runner_->world().rank() << std::endl;
        }

        void Worker::RecvUnevaluatedCase() {
            auto msg = MPIRunner::Message();
            msg.source = runner_->scheduler_rank_;
            msg.tag = MPIRunner::MsgTag::CASE_UNEVAL;
            runner_->RecvMessage(msg);
            current_case_ = msg.c;
        }

        void Worker::SendEvaluatedCase(MPIRunner::MsgTag tag) {
            auto msg = MPIRunner::Message();
            msg.destination = runner_->scheduler_rank_;
            msg.c = current_case_;
            msg.tag = MPIRunner::MsgTag::CASE_EVAL_SUCCESS;
            runner_->SendMessage(msg);
        }

        Optimization::Case *Worker::GetCurrentCase() {
            return current_case_;
        }

    }
}
