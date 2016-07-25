#include "synchronous_mpi_runner.h"

namespace Runner {

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
            overseer_ = std::unique_ptr<MPI::Overseer>(new MPI::Overseer(this));
        }
        else {
            InitializeSettings("rank" + QString::number(rank()));
            InitializeModel();
            InitializeSimulator();
            worker_ = std::unique_ptr<MPI::Worker>(new MPI::Worker(this));
        }
    }

    void SynchronousMPIRunner::Execute() {

    }
}
