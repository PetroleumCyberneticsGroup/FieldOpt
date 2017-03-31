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

#include "worker.h"

namespace Runner {
namespace MPI {

Worker::Worker(MPIRunner *runner) {
    runner_ = runner;
    runner_->RecvModelSynchronizationObject();
    std::cout << "Initialized Worker on " << runner_->world().rank() << std::endl;
}

void Worker::RecvUnevaluatedCase() {
    auto msg = MPIRunner::Message();
    msg.source = runner_->scheduler_rank_;
    msg.tag = MPIRunner::MsgTag::CASE_UNEVAL;
    runner_->RecvMessage(msg);
    current_case_ = msg.c;
}

void Worker::SendEvaluatedCase(MPIRunner::MsgTag tag) {
    auto msg = MPIRunner::Message();
    msg.destination = runner_->scheduler_rank_;
    msg.c = current_case_;
    msg.tag = MPIRunner::MsgTag::CASE_EVAL_SUCCESS;
    runner_->SendMessage(msg);
}

Optimization::Case *Worker::GetCurrentCase() {
    return current_case_;
}

}
}
