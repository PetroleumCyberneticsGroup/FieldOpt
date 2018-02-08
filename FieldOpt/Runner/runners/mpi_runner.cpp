#include "mpi_runner.h"
#include "Optimization/case_transfer_object.h"
#include "Model/model_synchronization_object.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
//#include <boost/mpi/status.hpp>
//#include <boost/lexical_cast.hpp>
//#include <iostream>
//#include <iomanip>

BOOST_IS_MPI_DATATYPE(boost::uuids::uuid)

using std::cout;
using std::endl;

namespace Runner {
namespace MPI {

MPIRunner::MPIRunner(RuntimeSettings *rts) : AbstractRunner(rts) {
  rank_ = world_.rank();
}

void MPIRunner::SendMessage(Message &message) {
  std::string s;
  if (message.c != nullptr) {
    auto cto = Optimization::CaseTransferObject(message.c);
    std::ostringstream oss;
    boost::archive::text_oarchive oa(oss);
    oa << cto;
    s = oss.str();
  }
  else {
    s = "";
  }
  world_.send(message.destination, message.tag, s);
  string dst, tag;
  dst = boost::lexical_cast<std::string>(message.destination);
  tag = boost::lexical_cast<std::string>(message.tag);
  printMessage("Sent a message to " + dst + " with tag " + tag
                   + " (" + tag_to_string[message.tag] + ")");
}

void MPIRunner::RecvMessage(Message &message) {
  Optimization::CaseTransferObject cto;
  string s, tag, src;
  tag = boost::lexical_cast<std::string>(message.tag);
  src = boost::lexical_cast<std::string>(message.source);
  printMessage("Waiting to receive a message with tag " + tag
                   + " (" + tag_to_string[message.tag] + ") "
                   + "from source " + src);
  mpi::status status = world_.recv(message.source, ANY_TAG, s);
  message.set_status(status);
  message.tag = status.tag();

  auto handle_received_case = [&]() mutable {
    std::istringstream iss(s);
    boost::archive::text_iarchive ia(iss);
    ia >> cto;
    message.c = cto.CreateCase();
  };

  if (message.tag == TERMINATE) {
    message.c = nullptr;
    printMessage("Received termination signal.");
    return;
  }
  else if (message.tag == MODEL_SYNC) {
    printMessage("Message w/ MODEL_SYNC tag received. "
                     "RecvMessage method cannot handle this. "
                     "Throwing exception.");
    throw std::runtime_error("RecvMessage is unable to handle model synchronization"
                                 "objects. This should be handled by the "
                                 "RecvModelSynchronizationObject method.");
  }
  else if (message.tag == CASE_UNEVAL) {
    printMessage("Received an unevaluated case.");
    handle_received_case();
  }
  else if (message.tag == CASE_EVAL_SUCCESS) {
    printMessage("Received a successfully evaluated case.");
    handle_received_case();
  }
  else if (message.tag == CASE_EVAL_INVALID) {
    printMessage("Received an invalid case.");
    handle_received_case();
  }
  else if (message.tag == CASE_EVAL_TIMEOUT) {
    printMessage("Received a case that was terminated due to timeout.");
  }
  else {
    printMessage("Received message with an unrecognized tag. Throwing exception.");
    throw std::runtime_error("RecvMessage received a message with an unrecognized tag.");
  }
}

void MPIRunner::BroadcastModel() {
  if (rank() != 0) {
    throw std::runtime_error(
        "BroadcastModel should only be called on the root process.");
  }
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
  if (rank() == 0) {
    std::runtime_error(
        "RecvModelSynchronizationObject should not be called on the root process.");
  }
  Model::ModelSynchronizationObject mso;
  std::string s;
  world_.recv(0, MODEL_SYNC, s);
  std::istringstream iss(s);
  boost::archive::text_iarchive ia(iss);
  ia >> mso;
  mso.UpdateVariablePropertyIds(model_);
}

void MPIRunner::printMessage(std::string message) {
  std::string time_stamp = QDateTime::currentDateTime().toString("hh:mm:ss").toStdString();

  int rank = world_.rank();
  string cstr;

//  if (rank % 6 == 0) cstr = BLRED;
//  else if (rank % 6 == 1) cstr = BLGREEN;
//  else if (rank % 6 == 2) cstr = BLYELLOW;
//  else if (rank % 6 == 3) cstr = BLBLUE;
//  else if (rank % 6 == 4) cstr = BLMAGENTA;
//  else if (rank % 6 == 5) cstr = BLCYAN;

  if (rank % 6 == 0) cstr = BRED;
  else if (rank % 6 == 1) cstr = BGREEN;
  else if (rank % 6 == 2) cstr = BYELLOW;
  else if (rank % 6 == 3) cstr = BBLUE;
  else if (rank % 6 == 4) cstr = BMAGENTA;
  else if (rank % 6 == 5) cstr = BCYAN;

  if (runtime_settings_->verb_vector()[0] >= 1) { // idx:0 -> run (Runner)
    cout << "[run]->" << cstr << "Rank=" << setfill('0') << setw(2)
         << setprecision(0) << rank << END << "<-" << time_stamp
         << ":---- " << message << endl;
    cout << fixed << setfill(' ') << setprecision(8);
  }

}
}
}
