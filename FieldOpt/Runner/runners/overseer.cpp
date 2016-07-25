#include "overseer.h"

namespace Runner {
    namespace MPI {
        Overseer::Overseer(MPIRunner *runner) {
            runner_ = runner;
            runner_->BroadcastModel();
            std::cout << "Initialized Overseer on " << runner_->world().rank() << std::endl;
        }
    }
}
