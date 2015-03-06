#include <QCoreApplication>
#include <boost/mpi.hpp>
#include <string>
#include <iostream>
#include <boost/serialization/string.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "transferobjects/modelperturbation.h"
#include "transferobjects/simulationresults.h"
#include "masterrunner.h"
#include "simulationlauncher.h"

namespace mpi = boost::mpi;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    mpi::environment env(argc, argv);
    mpi::communicator world;

    if (world.rank() == 0) {
        MasterRunner runner = MasterRunner(&world);
        runner.initialize();
        std::cout << "Initialized MasterRunner" << std::endl;
        runner.perturbModel();
        std::cout << "Perturbed model" << std::endl;
        runner.distributePerturbations();
        std::cout << "Distributed perturbations" << std::endl;
        runner.determineOptimal();
    }
    else {
        SimulationLauncher simlauncher = SimulationLauncher(&world);
        simlauncher.initialize();
        std::cout << "Initialized SimulationLauncher" << std::endl;
        simlauncher.receivePerturbations();
        simlauncher.startSimulation();
    }
    return 0;
}
