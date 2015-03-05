#include <QCoreApplication>
#include <boost/mpi.hpp>
#include <string>
#include <iostream>
#include <boost/serialization/string.hpp>

namespace mpi = boost::mpi;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    mpi::environment env(argc, argv);
    mpi::communicator world;

    if (world.rank() == 0) {
        world.send(1, 0, std::string("Hello"));
        std::string message;
        world.recv(1, 1, message);
        std::cout << message << "!" << std::endl;
    }
    else {
        std::string message;
        world.recv(0, 0, message);
        std::cout << message << ", ";
        std::cout.flush();
        world.send(0, 1, std::string("world"));
    }
    return 0;
}
