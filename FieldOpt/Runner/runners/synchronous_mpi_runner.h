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

#ifndef FIELDOPT_SYNCHRONOUS_MPI_RUNNER_H
#define FIELDOPT_SYNCHRONOUS_MPI_RUNNER_H

#include "mpi_runner.h"
#include "overseer.h"
#include "worker.h"

namespace Runner {
namespace MPI {

/*!
 * @brief The SynchronousMPIRunner class performs the 
 * optimization synchronously in parallel. Depending
 * on the process rank, it will instantiate either an 
 * Overseer (rank = 0) to handle optimizer iteraction
 * and logging, or a Worker (rank > 0) to execute 
* simulations.
 *
 * Still todo:
 *   - Probably some more logging and console debug messages
 *   - Logging of runner stats
 */
class SynchronousMPIRunner : public MPIRunner, public Loggable {
 public:
  LogTarget GetLogTarget() override;
  SynchronousMPIRunner(RuntimeSettings *rts);
  map<string, string> GetState() override;
  QUuid GetId() override;

  virtual void Execute();

 private:
  MPI::Overseer *overseer_;
  MPI::Worker *worker_;

  bool model_update_done_;
  bool simulation_done_;

  /*!
   * @brief Distribute cases to be evaluated to all but one worker.
   */
  void initialDistribution();


};

}
}

#endif //FIELDOPT_SYNCHRONOUS_MPI_RUNNER_H
