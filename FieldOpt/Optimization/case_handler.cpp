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

// -----------------------------------------------------------------
#include "case_handler.h"
#include <iostream>

// -----------------------------------------------------------------
namespace Optimization {

// -----------------------------------------------------------------
CaseHandler::CaseHandler(Settings::Optimizer *settings) {
  cases_ = QHash<QUuid, Case *>();
  evaluation_queue_ = QQueue<QUuid>();
  evaluating_ = QList<QUuid>();
  evaluated_ = QList<QUuid>();
  evaluated_recently_ = QList<QUuid>();

  nr_totl_ = 0;
  nr_eval_ = 0;
  nr_bkpd_ = 0;
  nr_timo_ = 0;
  nr_invl_ = 0;
  nr_fail_ = 0;

  settings_ = settings;
}

// -----------------------------------------------------------------
CaseHandler::CaseHandler(Case *base_case,
                         Settings::Optimizer *settings) {
  cases_[base_case->id()] = base_case;
  evaluated_.append(base_case->id());
  settings_ = settings;
}

// -----------------------------------------------------------------
void CaseHandler::AddNewCase(Case *c) {
  c->state.queue = Case::CaseState::QueueStatus::Q_QUEUED;
  evaluation_queue_.enqueue(c->id());
  cases_[c->id()] = c;
  nr_totl_++;
}

// -----------------------------------------------------------------
void CaseHandler::AddNewCases(QList<Case *> cases) {
  for (Case *c : cases) {
    c->state.queue = Case::CaseState::QueueStatus::Q_QUEUED;
    AddNewCase(c);
  }
}

// -----------------------------------------------------------------
Case *CaseHandler::GetNextCaseForEvaluation() {
  if (evaluation_queue_.size() == 0)
    throw CaseHandlerException(
        "The evaluation queue contains no cases.");
  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Size of eval.queue:------ "
         << evaluation_queue_.length() << endl;
  evaluating_.append(evaluation_queue_.head());

  cases_[evaluation_queue_.head()]->state.queue =
      Case::CaseState::QueueStatus::Q_DEQUEUED;

  return cases_[evaluation_queue_.dequeue()];
}

// -----------------------------------------------------------------
void CaseHandler::SetCaseEvaluated(const QUuid id) {
  if (!evaluating_.contains(id))
    throw CaseHandlerException(
        "The case id is not found in the list of cases being evaluated.");

  evaluating_.removeAll(id);
  evaluated_.append(id);
  evaluated_recently_.append(id);

  switch (cases_[id]->state.eval) {
    case Case::CaseState::EvalStatus::E_DONE: nr_eval_++; break;
    case Case::CaseState::EvalStatus::E_BOOKKEEPED: nr_bkpd_++; break;
    case Case::CaseState::EvalStatus::E_TIMEOUT: nr_timo_++; break;
    case Case::CaseState::EvalStatus::E_FAILED: nr_fail_++; break;
  }
  if (cases_[id]->state.err_msg != Case::CaseState::ErrorMessage::ERR_OK){
    nr_invl_++;
  }
}

// -----------------------------------------------------------------
void CaseHandler::UpdateCaseObjectiveFunctionValue(const QUuid id,
                                                   const double ofv) {
  cases_[id]->set_objective_function_value(ofv);
}

// -----------------------------------------------------------------
void CaseHandler::SetCaseState(QUuid id,
                               Case::CaseState state,
                               int wic_time, int sim_time) {
  cases_[id]->state = state;
  cases_[id]->SetWICTime(wic_time);
  cases_[id]->SetSimTime(sim_time);
}

// -----------------------------------------------------------------
QList<Case *> CaseHandler::RecentlyEvaluatedCases() const {
  QList<Case *> recently_evaluated_cases = QList<Case *>();
  for (QUuid id : evaluated_recently_) {
    recently_evaluated_cases.append(cases_[id]);
  }
  return recently_evaluated_cases;
}

// -----------------------------------------------------------------
void CaseHandler::ClearRecentlyEvaluatedCases() {
  if (evaluated_recently_.size() > 0)
    evaluated_recently_.clear();
}

// -----------------------------------------------------------------
QList<Case *> CaseHandler::AllCases() const {
  return cases_.values();
}

// -----------------------------------------------------------------
QList<Case *> CaseHandler::QueuedCases() const {
  QList<Case *> queued_cases = QList<Case *>();
  for (QUuid id : evaluation_queue_) {
    queued_cases.append(cases_[id]);
  }
  return queued_cases;
}

// -----------------------------------------------------------------
QList<Case *> CaseHandler::CasesBeingEvaluated() const {
  QList<Case *> cases_being_evaluated = QList<Case *>();
  for (QUuid id : evaluating_) {
    cases_being_evaluated.append(cases_[id]);
  }
  return cases_being_evaluated;
}

// -----------------------------------------------------------------
QList<Case *> CaseHandler::EvaluatedCases() const {
  QList<Case *> evaluated_cases = QList<Case *>();
  for (QUuid id : evaluated_) {
    evaluated_cases.append(cases_[id]);
  }
  return evaluated_cases;
}

// -----------------------------------------------------------------
void CaseHandler::DequeueCase(QUuid id) {
  cases_[id]->state.queue = Case::CaseState::QueueStatus::Q_DISCARDED;
  evaluation_queue_.removeOne(id);
}

// -----------------------------------------------------------------
Case *CaseHandler::GetCase(const QUuid id) const {
  return cases_[id];
}

}
