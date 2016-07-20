#ifndef FIELDOPT_MPIRUNNER_H
#define FIELDOPT_MPIRUNNER_H

#include "abstract_runner.h"
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
namespace mpi = boost::mpi;

namespace Runner {

    /*!
     * @brief The MPIRunner class is the base class for runners using MPI for parallelization.
     */
    class MPIRunner : public AbstractRunner {

    protected:
        MPIRunner(RuntimeSettings *rts);
        mpi::environment env_;
        mpi::communicator world_;
    };

}


#endif //FIELDOPT_MPIRUNNER_H
