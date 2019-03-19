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
#include "overseer.h"
#include <boost/lexical_cast.hpp>
#include <chrono>
#include <thread>

namespace Runner {
namespace MPI {
Overseer::Overseer(MPIRunner *runner) {
    runner_ = runner;
    runner_->BroadcastModel();

    for (int i = 1; i < runner->world_.size(); ++i) {
        workers_.insert(i, new WorkerStatus(i));
    }
    runner_->printMessage("Initialized overseer.");
    last_sim_start_ = current_time();
}

void Overseer::AssignCase(Optimization::Case *c, int preferred_worker) {
    if (NumberOfFreeWorkers() == 0) throw std::runtime_error("Cannot assign Case. No free workers found.");
    WorkerStatus *worker;
    if (preferred_worker > 0) {
        worker = workers_[preferred_worker];
        assert(worker->working == false);
    }
    else {
        worker = getFreeWorker();
    }
    if (time_since_seconds(last_sim_start_) < runner_->SimulatorDelay()) {
        runner_->printMessage("Waiting for simulator delay ...", 2);
        std::this_thread::sleep_until(last_sim_start_ + chrono::seconds(runner_->SimulatorDelay()));
    }
    auto msg = MPIRunner::Message();
    msg.tag = MPIRunner::MsgTag ::CASE_UNEVAL;
    msg.destination = worker->rank;
    msg.c = c;
    runner_->SendMessage(msg);
    worker->start();
    last_sim_start_ = current_time();
    c->state.eval = Optimization::Case::CaseState::EvalStatus::E_CURRENT;
    runner_->printMessage("Assigned case to worker " + boost::lexical_cast<std::string>(worker->rank), 2);
    runner_->printMessage("Current status for workers:\n" + workerStatusSummary(), 2);
}

Optimization::Case *Overseer::RecvEvaluatedCase() {
    auto message = MPIRunner::Message();
    runner_->RecvMessage(message);
    workers_[message.source]->stop();
    runner_->printMessage("Received case with tag " + boost::lexical_cast<std::string>(message.tag)
                              + " from worker " + boost::lexical_cast<std::string>(message.source), 2);
    runner_->printMessage("Current status for workers:\n" + workerStatusSummary(), 2);
    last_case_tag = message.get_tag();
    return message.c;
}

Overseer::WorkerStatus * Overseer::getFreeWorker() {
    if (NumberOfFreeWorkers() == 0) throw std::runtime_error("No free workers in network.");
    for (int i = 1; i < runner_->world_.size(); ++i) {
        if (workers_[i]->working == false) return workers_[i];
    }
}

int Overseer::NumberOfFreeWorkers() {
    int f = 0;
    for (int i = 1; i < runner_->world_.size(); ++i) {
        if (workers_[i]->working == false)
            f++;
    }
    return f;
}

Overseer::WorkerStatus * Overseer::GetLongestRunningWorker() {
    if (NumberOfFreeWorkers() == runner_->world_.size()-1) return nullptr;
    int longest_running_time = -1;
    int longest_running_worker;
    for (int i = 1; i < runner_->world_.size(); ++i) {
        if (workers_[i]->working_seconds() > longest_running_time) {
            longest_running_time = workers_[i]->working_seconds();
            longest_running_worker = workers_[i]->rank;
        }
    }
    return workers_[longest_running_worker];
}

void Overseer::TerminateWorkers() {
    for (int i = 1; i < runner_->world_.size(); ++i) {
        auto msg = MPIRunner::Message();
        msg.destination = i;
        msg.tag = MPIRunner::MsgTag::TERMINATE;
        runner_->SendMessage(msg);
    }
}

void Overseer::EnsureWorkerTermination() {
    for (int i = 1; i < runner_->world_.size(); ++i) {
        auto msg = MPIRunner::Message();
        msg.tag = MPIRunner::MsgTag::TERMINATE;
        msg.source = i;
        runner_->RecvMessage(msg);
        if (msg.tag != MPIRunner::MsgTag::TERMINATE)
            throw runtime_error("Something's fishy in the termination.");
    }
}

int Overseer::NumberOfBusyWorkers() {
    return workers_.count() - NumberOfFreeWorkers();
}

std::vector<int> Overseer::GetFreeWorkerRanks() const {
    std::vector<int> free_workers;
    for (int i = 1; i < runner_->world_.size(); ++i) {
        if (workers_[i]->working == false)
            free_workers.push_back(workers_[i]->rank);
    }
    return free_workers;
}

std::string Overseer::workerStatusSummary() {
    QString status = "";
    for (auto stat : workers_.values()) {
        if (stat->working)
            status.append(QString("\tWorker %1: working. Duration of current task: %2 sec\n")
                              .arg(stat->rank)
                              .arg(stat->working_seconds())
            );
        else
            status.append(QString("\tWorker %1: idle").arg(stat->rank));
    }
    return status.toStdString();
}
}
}
