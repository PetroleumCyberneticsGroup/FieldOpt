/******************************************************************************
 *
 *
 *
 * Created: 30.11.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#ifndef CASE_HANDLER_H
#define CASE_HANDLER_H

#include "case.h"
#include <QQueue>

namespace Optimization {

/*!
 * \brief The CaseHandler class acts as a handler for cases for the optimizer. It keeps track of the cases
 * that have been evaluated and the ones that have not.
 */
class CaseHandler
{
public:
    CaseHandler();
    CaseHandler(Case *base_case); //!< Call the default constructor and add the base case to list of evaluated cases.

    /*!
     * \brief AddNewCase Add a new non-evaluated case to the queue.
     */
    void AddNewCase(Case *c);

    /*!
     * \brief AddNewCases Add any number of non-evaluated cases to the queue.
     */
    void AddNewCases(QList<Case *> cases);

    /*!
     * \brief GetNextCaseForEvaluation Get the next case to be evaluated.
     *
     * The returned case is also added to the list of cases currently being evaluated.
     */
    Case *GetNextCaseForEvaluation();

    /*!
     * \brief SetCaseEvaluated Mark a case as evaluated.
     *
     * The case is moved to the list of evaluated cases.
     * \param id The id/key of the case to set as evaluated.
     */
    void SetCaseEvaluated(const QUuid id);

    /*!
     * \brief RecentlyEvaluatedCases Get the list of cases that has been marked as evaluated since the last
     * time ClearRecentlyEvaluatedCases() was called.
     */
    QList<Case *> RecentlyEvaluatedCases() const;

    /*!
     * \brief ClearRecentlyEvaluatedCases Clear the list of recently evaluated cases. Should be called whenever
     * a significant point is reached by the optimizer, for example at the end of an iteration.
     */
    void ClearRecentlyEvaluatedCases();

    /*!
     * \brief QueuedCases Get the list of cases currently queued to be evaluated.
     */
    QList<Case *> QueuedCases() const;

    /*!
     * \brief CasesBeingEvaluated Get the list of cases currently being evaluated.
     * \return
     */
    QList<Case *> CasesBeingEvaluated() const;

    /*!
     * \brief EvaluatedCases Get the list of cases that have been marked as evaluated.
     */
    QList<Case *> EvaluatedCases() const;

private:
    QQueue<QUuid> evaluation_queue_; //!< Queue of the next keys to be evaluated.
    QList<QUuid> evaluating_; //!< List of keys for Cases currently being evaluated.
    QList<QUuid> evaluated_; //!< List of keys for Cases that have already been evaluated.
    QList<QUuid> evaluated_recently_; //!< List of keys that have recently been evaluated.
    QHash<QUuid, Case *> cases_;
};

}

#endif // CASE_HANDLER_H
