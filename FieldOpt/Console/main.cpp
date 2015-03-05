#include <QCoreApplication>
#include <boost/mpi.hpp>
#include <string>
#include <iostream>
#include <boost/serialization/string.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "transferobjects/modelperturbation.h"
#include "transferobjects/simulationresults.h"

namespace mpi = boost::mpi;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    mpi::environment env(argc, argv);
    mpi::communicator world;

    if (world.rank() == 0) {
        ModelPerturbation perturbation = ModelPerturbation(1, 3, 25);
        std::cout << "Process " << world.rank() << " created perturbation." << std::endl;
        std::cout << perturbation.toString() << std::endl;

        world.send(1, 0, perturbation);
        std::cout << "Sent process object to process 1, with id 1" << std::endl;

        SimulationResults results;
        world.recv(1, 1, results);
        std::cout << "Received results on process " << world.rank() << "\n";
        std::cout << results.toString() << std::endl;
    }
    else {
        ModelPerturbation perturbation;
        world.recv(0, 0, perturbation);
        std::cout << "Process " << world.rank() << " received perturbation.\n";
        std::cout << perturbation.toString() << std::endl;

        SimulationResults results = SimulationResults(1, 2030);
        world.send(0, 1, results);
        std::cout << "Sent results from process " << world.rank() << "\n";
        std::cout << results.toString() << std::endl;
    }
    return 0;
}
