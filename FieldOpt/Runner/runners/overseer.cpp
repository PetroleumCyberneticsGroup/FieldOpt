#include "overseer.h"

namespace Runner {
    namespace MPI {
        Overseer::Overseer(MPIRunner *runner) {
            runner_ = runner;
            std::cout << "Initialized Overseer on " << runner_->world().rank() << std::endl;
        }
    }
}
