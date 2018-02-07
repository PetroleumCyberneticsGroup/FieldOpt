/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef FIELDOPT_WOKER_H
#define FIELDOPT_WOKER_H

#include "mpi_runner.h"

namespace Runner {
namespace MPI {
/*!
 * @brief The Worker class is responsible for receiving and sending
 * from/to an overseer object. The runner taken as a parameter in
 * the constructor is primarily used for the common MPI helpers.
 */
class Worker {
 public:
  Worker(MPIRunner *runner);

  /*!
   * @brief Receive an unevaluated case from the Scheduler
   * and set it as the current_case_.
   */
  void RecvUnevaluatedCase();

  /*!
   * @brief Send the current_case_ back to the Scheduler.
   * @param tag The tag the message should be sent with,
   * indicating whether the evaluation was successful.
   */
  void SendEvaluatedCase(MPIRunner::MsgTag tag);

  /*!
   * @brief Send a message to the overseer confirming finalization.
   */
  void ConfirmFinalization();

  Optimization::Case *GetCurrentCase();

  int GetCurrentRank() { return runner_->scheduler_rank_; }

  MPIRunner::MsgTag GetCurrentTag() { return current_tag_; }

 private:
  MPIRunner *runner_;
  Optimization::Case *current_case_;
  MPIRunner::MsgTag current_tag_;
};
}
}

#endif //FIELDOPT_WOKER_H
