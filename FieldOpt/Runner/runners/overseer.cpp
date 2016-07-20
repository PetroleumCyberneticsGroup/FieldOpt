#include "overseer.h"

namespace Runner {
    namespace MPI {
        Overseer::Overseer(MPIRunner *runner) {
            runner_ = runner;
        }
    }
}
