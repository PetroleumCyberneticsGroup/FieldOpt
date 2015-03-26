#include <QCoreApplication>
#include <QString>
#include <boost/mpi.hpp>
#include <string>
#include <iostream>
#include <boost/serialization/string.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "transferobjects/modelperturbation.h"
#include "transferobjects/simulationresults.h"
#include "masterrunner.h"
#include "simulationlauncher.h"
#include "fileio/readeres/driverreader.h"
#include "parallelprinter.h"

namespace mpi = boost::mpi;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    mpi::environment env(argc, argv);
    mpi::communicator world;
    ParallelPrinter printer = ParallelPrinter(world.rank());

    if (world.size() < 2 && world.rank() == 0) {
        std::cerr << "============================== ERROR ==============================" << std::endl
                  << "Needs to be run with at least 2 MPI processes." << std::endl
                  << "Example of how to start the program:" << std::endl
                  << "mpirun -n 2 -x LD_LIBRARY_PATH Console /path/to/driver.dat" << std::endl
                  << "===================================================================" << std::endl;
        return (1);
    }
    else if (world.size() < 2 && world.rank() > 0) {
        return (1);
    }

    // The program should take one and only one argument.
    if (argc != 2 && world.rank() == 0) {
        std::cerr << "============================== ERROR ==============================" << std::endl
                  << "This program should take one and only one argument." << std::endl
                  << "This argument should be the path to the driver file to be used." << std::endl
                  << "Example of how to start the program:" << std::endl
                  << "mpirun -n 2 -x LD_LIBRARY_PATH Console /path/to/driver.dat" << std::endl
                  << "===================================================================" << std::endl;
        return (1);
    }
    else if (argc != 2 && world.rank() > 0) {
        return (1);
    }
    else if (argc == 2 && world.rank() == 0) {
        std::cout << "===================================================================" << std::endl
                  << "Running program with the following driver file path: " << std::endl
                  << argv[1] << std::endl
                  << "===================================================================" << std::endl;
    }

    QString driverPath = QString::fromStdString(argv[1]);

    if (world.rank() == 0) {
        MasterRunner runner = MasterRunner(&world);
        runner.initialize(driverPath);
        printer.print(QString("Initialized MasterRunner"), false);
        runner.perturbModel();
        printer.print(QString("Perturbed model."), false);
        runner.distributePerturbations();
        printer.print(QString("Distributed perturbations"), false);
        runner.determineOptimal();
    }
    else {
        SimulationLauncher simlauncher = SimulationLauncher(&world);
        simlauncher.initialize(driverPath);
        printer.print(QString("Initialized SimulationLauncher"), false);
        simlauncher.receivePerturbations();
        simlauncher.startSimulation();
    }
    return 0;
}
