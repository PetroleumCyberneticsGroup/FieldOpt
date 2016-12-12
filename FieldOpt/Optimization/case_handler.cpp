#include "case_handler.h"

namespace Optimization {

    CaseHandler::CaseHandler()
    {
        cases_ = QHash<QUuid, Case *>();
        evaluation_queue_ = QQueue<QUuid>();
        evaluating_ = QList<QUuid>();
        evaluated_ = QList<QUuid>();
        evaluated_recently_ = QList<QUuid>();
    }

    CaseHandler::CaseHandler(Case *base_case)
            : CaseHandler()
    {
        cases_[base_case->id()] = base_case;
        evaluated_.append(base_case->id());
    }

    void CaseHandler::AddNewCase(Case *c)
    {
        evaluation_queue_.enqueue(c->id());
        cases_[c->id()] = c;
    }

    void CaseHandler::AddNewCases(QList<Case *> cases)
    {
        for (Case *c : cases) {
            AddNewCase(c);
        }
    }

    Case *CaseHandler::GetNextCaseForEvaluation()
    {
        if (evaluation_queue_.size() == 0) throw CaseHandlerException("The evaluation queue contains no cases.");
        evaluating_.append(evaluation_queue_.head());
        return cases_[evaluation_queue_.dequeue()];
    }

    void CaseHandler::SetCaseEvaluated(const QUuid id)
    {
        if (!evaluating_.contains(id)) throw CaseHandlerException("The case id is not found in the list of cases being evaluated.");
        evaluating_.removeAll(id);
        evaluated_.append(id);
        evaluated_recently_.append(id);
    }

    void CaseHandler::UpdateCaseObjectiveFunctionValue(const QUuid id, const double ofv) {
        cases_[id]->set_objective_function_value(ofv);
    }

    QList<Case *> CaseHandler::RecentlyEvaluatedCases() const
    {
        QList<Case *> recently_evaluated_cases = QList<Case *>();
        for (QUuid id : evaluated_recently_) {
            recently_evaluated_cases.append(cases_[id]);
        }
        return recently_evaluated_cases;
    }

    void CaseHandler::ClearRecentlyEvaluatedCases()
    {
        if (evaluated_recently_.size() > 0)
            evaluated_recently_.clear();
    }

    QList<Case *> CaseHandler::QueuedCases() const
    {
        QList<Case *> queued_cases = QList<Case *>();
        for (QUuid id : evaluation_queue_) {
            queued_cases.append(cases_[id]);
        }
        return queued_cases;
    }

    QList<Case *> CaseHandler::CasesBeingEvaluated() const
    {
        QList<Case *> cases_being_evaluated = QList<Case *>();
        for (QUuid id : evaluating_) {
            cases_being_evaluated.append(cases_[id]);
        }
        return cases_being_evaluated;
    }

    QList<Case *> CaseHandler::EvaluatedCases() const
    {
        QList<Case *> evaluated_cases = QList<Case *>();
        for (QUuid id : evaluated_) {
            evaluated_cases.append(cases_[id]);
        }
        return evaluated_cases;
    }

    void CaseHandler::DequeueCase(QUuid id) {
        evaluation_queue_.removeOne(id);
    }

    Case *CaseHandler::GetCase(const QUuid id) const {
        return cases_[id];
    }
}
