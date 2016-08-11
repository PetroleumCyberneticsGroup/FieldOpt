#include "mpi_runner.h"
#include "Optimization/case_transfer_object.h"
#include "Model/model_synchronization_object.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/mpi/status.hpp>
#include <iostream>

BOOST_IS_MPI_DATATYPE(boost::uuids::uuid)

namespace Runner {
    namespace MPI {

        MPIRunner::MPIRunner(RuntimeSettings *rts) : AbstractRunner(rts) {
            rank_ = world_.rank();
        }

        void MPIRunner::SendCase(Optimization::Case *c, int dest, MPIRunner::MsgTag tag) {
            auto cto = Optimization::CaseTransferObject(c);
            std::ostringstream oss;
            boost::archive::text_oarchive oa(oss);
            oa << cto;
            std::string s = oss.str();
            world_.send(dest, tag, s);
        }

        Optimization::Case *MPIRunner::RecvCase(int &source, MPIRunner::MsgTag tag) {
            Optimization::CaseTransferObject cto;
            std::string s;
            if (tag == TERMINATE)
                return nullptr;
            else if (tag == CASE_EVAL) {
                mpi::status status = world_.recv(mpi::any_source, tag, s);
                source = status.source();
            }
            else {
                mpi::status status = world_.recv(source, tag, s);
            }
            std::istringstream iss(s);
            boost::archive::text_iarchive ia(iss);
            ia >> cto;
            auto c = cto.CreateCase();
            return c;
        }

        void MPIRunner::BroadcastModel() {
            if (rank() != 0) throw std::runtime_error("BroadcastModel should only be called on the root process.");
            auto mso = Model::ModelSynchronizationObject(model_);
            std::ostringstream oss;
            boost::archive::text_oarchive oa(oss);
            oa << mso;
            std::string s = oss.str();
            for (int r = 1; r < world_.size(); ++r) {
                world_.send(r, MODEL_SYNC, s);
            }
        }

        void MPIRunner::RecvModelSynchronizationObject() {
            if (rank() == 0) std::runtime_error("BroadcastModel should not be called on the root process.");
            Model::ModelSynchronizationObject mso;
            std::string s;
            world_.recv(0, MODEL_SYNC, s);
            std::istringstream iss(s);
            boost::archive::text_iarchive ia(iss);
            ia >> mso;
            mso.UpdateVariablePropertyIds(model_);
        }

        void MPIRunner::printMessage(std::string message, int min_verb) {
            if (runtime_settings_->verbosity_level() >= min_verb)
                std::cout << "RANK " << world_.rank() << ": " << message << std::endl;
        }
    }
}
