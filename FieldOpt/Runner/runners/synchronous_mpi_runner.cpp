#include "synchronous_mpi_runner.h"

namespace Runner {

    SynchronousMPIRunner::SynchronousMPIRunner(RuntimeSettings *rts) : MPIRunner(rts) {
        assert(world_.size() >= 2 && "The SynchronousMPIRunner requires at least two MPI processes.");

        if (world_.rank() == 0) {
            std::cout << "00" << std::endl;
            InitializeSettings("rank" + QString::number(rank()));

            InitializeModel();
            std::cout << "01" << std::endl;
            InitializeSimulator();
            std::cout << "02" << std::endl;
            EvaluateBaseModel();
            std::cout << "03" << std::endl;
            InitializeObjectiveFunction();
            std::cout << "04" << std::endl;
            InitializeBaseCase();
            std::cout << "05" << std::endl;
            InitializeOptimizer();
            std::cout << "06" << std::endl;
            InitializeBookkeeper();
            std::cout << "07" << std::endl;
            InitializeLogger();
            std::cout << "08" << std::endl;
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
