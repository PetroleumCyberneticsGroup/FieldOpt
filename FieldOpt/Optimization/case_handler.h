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
#ifndef CASE_HANDLER_H
#define CASE_HANDLER_H

// -----------------------------------------------------------------
#include "case.h"
#include <QQueue>
#include <Settings/settings.h>

// -----------------------------------------------------------------
namespace Optimization {

// -----------------------------------------------------------------
/*!
 * \brief The CaseHandler class acts as a handler for cases for the
 * optimizer. It keeps track of the cases that have been evaluated
 * and the ones that have not.
 */
class CaseHandler
{
 public:
  CaseHandler(Settings::Optimizer *settings);

  // ---------------------------------------------------------------
  /*!
 * \brief Call the default constructor and add the base case to
   * list of evaluated cases.
 */
  CaseHandler(Case *base_case, Settings::Optimizer *settings);

  // ---------------------------------------------------------------
  /*!
   * \brief AddNewCase Add a new non-evaluated case to the queue.
   */
  void AddNewCase(Case *c);

  // ---------------------------------------------------------------
  /*!
   * \brief AddNewCases Add any number of non-evaluated cases to
   * the queue.
   */
  void AddNewCases(QList<Case *> cases);

  // ---------------------------------------------------------------
  /*!
   * \brief GetNextCaseForEvaluation Get next case to be
   * evaluated.
   *
   * The returned case is also added to the list of cases
   * currently being evaluated.
   */
  Case *GetNextCaseForEvaluation();

  // ---------------------------------------------------------------
  /*!
   * \brief SetCaseEvaluated Mark a case as evaluated.
   *
   * The case is moved to the list of evaluated cases.
   * \param id The id/key of the case to set as evaluated.
   */
  void SetCaseEvaluated(const QUuid id);

  // ---------------------------------------------------------------
  /*!
   * @brief Get a case by ID.
   */
  Case *GetCase(const QUuid id) const;

  // ---------------------------------------------------------------
  /*!
   * \brief UpdateCaseObjectiveFunctionValue updates the
   * objective function value of a case. This is needed
   * when using, for instance, MPI based runners, where
   * the case object before and after evaluation is not
   * the same one.
   *
   * \param id The ID of the case to be updated.
   * \param ofv The objective function value to be set
   * for the case.
   */
  void UpdateCaseObjectiveFunctionValue(const QUuid id, const double ofv);

  // ---------------------------------------------------------------
  /*!
   * @brief Update the evaluation status, error message, and WIC time
   * for a case.
   * @param id The id of the case to set the status for.
   * @param state The state to be set.
   * @param wic_time The well index calculation time.
   * @param sim_time The simulator execution time.
   */
  void SetCaseState(QUuid id, Case::CaseState state, int wic_time, int sim_time);

  // ---------------------------------------------------------------
  /*!
   * \brief RecentlyEvaluatedCases Get the list of cases that has been
   * marked as evaluated since the last time ClearRecentlyEvaluatedCases()
   * was called.
   *
   */
  QList<Case *> RecentlyEvaluatedCases() const;

  // ---------------------------------------------------------------
  /*!
   * \brief ClearRecentlyEvaluatedCases Clear the list of recently
   * evaluated cases. Should be called whenever a significant point
   * is reached by the optimizer, for example at the end of an
   * iteration.
   *
   */
  void ClearRecentlyEvaluatedCases();

  // ---------------------------------------------------------------
  /*!
   * \brief QueuedCases Get list of cases currently queued to be evaluated.
   */
  QList<Case *> QueuedCases() const;

  // ---------------------------------------------------------------
  /*!
   * \brief CasesBeingEvaluated Get list of cases currently being evaluated.
   * \return
   */
  QList<Case *> CasesBeingEvaluated() const;

  // ---------------------------------------------------------------
  /*!
   * \brief EvaluatedCases Get list of cases marked as evaluated.
   */
  QList<Case *> EvaluatedCases() const;

  // ---------------------------------------------------------------
  /*!
   * @brief Get _all_ cases.
   */
  QList<Case *> AllCases() const;

  // ---------------------------------------------------------------
  /*!
   * @brief Dequeue a case that will not be evaluated.
   * @param id UUID of case to be dequeued.
   */
  void DequeueCase(QUuid id);

  // ---------------------------------------------------------------
  int NumberTotal() const { return nr_totl_; }
  int NumberSimulated() const { return nr_eval_; }
  int NumberBookkeeped() const { return nr_bkpd_; }
  int NumberTimeout() const { return  nr_timo_; }
  int NumberInvalid() const { return nr_invl_; }
  int NumberFailed() const { return nr_fail_; }

 private:
  QQueue<QUuid> evaluation_queue_; //!< Queue of the next keys to be evaluated.
  QList<QUuid> evaluating_; //!< List of keys for Cases currently being evaluated.
  QList<QUuid> evaluated_; //!< List of keys for Cases that have already been evaluated.
  QList<QUuid> evaluated_recently_; //!< List of keys that have recently been evaluated.
  QHash<QUuid, Case *> cases_;

  int nr_totl_; //!< Total number of cases added to handler.
  int nr_eval_; //!< Number of cases that have been simulated.
  int nr_bkpd_; //!< Number of cases handled by bookkeeper.
  int nr_timo_; //!< Number of cases interrupted because of timeout.
  int nr_invl_; //!< Number of invalid cases (failed while being applied to model).
  int nr_fail_; //!< Number of cases that have failed for some reason.

  Settings::Optimizer *settings_;
};

}

#endif // CASE_HANDLER_H
