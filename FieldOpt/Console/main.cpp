#include <QCoreApplication>
#include <QString>
#include <boost/mpi.hpp>
#include <string>
#include <iostream>
#include <boost/serialization/string.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "masterrunner.h"
#include "simulationlauncher.h"
#include "Library/fileio/readeres/driverreader.h"
#include "parallelprinter.h"
#include "Library/exceptionhandler/exceptionhandler.h"

namespace mpi = boost::mpi;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    mpi::environment env(argc, argv);
    mpi::communicator world;
    ParallelPrinter printer = ParallelPrinter(world.rank());
    QString mrstPath;  //!< Holds the MRST path if it is passed as a parameter

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

    // The program should take at least one argument.
    if (argc < 2 && world.rank() == 0) {
        std::cerr << "============================== ERROR ==============================" << std::endl
                  << "This program should take at least one argument." << std::endl
                  << "This argument should be the path to the driver file to be used." << std::endl
                  << "Example of how to start the program:" << std::endl
                  << "mpirun -n 2 -x LD_LIBRARY_PATH Console /path/to/driver.dat" << std::endl
                  << "===================================================================" << std::endl;
        return (1);
    }
    else if (argc < 2 && world.rank() > 0) {
        return (1);
    }
    else if (argc >= 2 && QString::compare(argv[2], "-verbose") == 0 ) {
        std::cout << "Setting verbose mode." << std::endl;
        ExceptionHandler::verbose = true;
    }
    else if (argc >= 2) {  // If the second parameter is not "-verbose", assuming that it is the path to the MRST directory
        std::cout << "Setting MRST directory to " << argv[2] << std::endl;
        mrstPath = QString(argv[2]);
    }
    else if (argc >= 2 && world.rank() == 0) {
        std::cout << "===================================================================" << std::endl
                  << "Running program with the following driver file path: " << std::endl
                  << argv[1] << std::endl
                  << "===================================================================" << std::endl;
    }

    QString driverPath = QString::fromStdString(argv[1]);

    if (world.rank() == 0) {
        MasterRunner runner = MasterRunner(&world);
        runner.initialize(driverPath);
        runner.start();
    }
    else {
        if (mrstPath.startsWith("/")) {
            SimulationLauncher simlauncher = SimulationLauncher(&world, mrstPath);
            simlauncher.initialize(driverPath);
            printer.print(QString("Initialized SimulationLauncher"), false);
            simlauncher.start();
        }
        else {
            SimulationLauncher simlauncher = SimulationLauncher(&world);
            simlauncher.initialize(driverPath);
            printer.print(QString("Initialized SimulationLauncher"), false);
            simlauncher.start();
        }
    }
    return 0;
}
