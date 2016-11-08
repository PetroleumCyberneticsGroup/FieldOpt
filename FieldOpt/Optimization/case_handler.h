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
         * \brief UpdateCaseObjectiveFunctionValue updates the objective function value of a
         * case. This is needed when using, for instance, MPI based runners, where the case
         * object before and after evaluation is not the same one.
         * \param id The ID of the case to be updated.
         * \param ofv The objective function value to be set for the case.
         */
        void UpdateCaseObjectiveFunctionValue(const QUuid id, const double ofv);

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
