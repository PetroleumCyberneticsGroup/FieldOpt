#include "worker.h"

namespace Runner {
    namespace MPI {

        Worker::Worker(MPIRunner *runner) {
            runner_ = runner;
            runner_->RecvModelSynchronizationObject();
            std::cout << "Initialized Worker on " << runner_->world().rank() << std::endl;
        }

    }
}
