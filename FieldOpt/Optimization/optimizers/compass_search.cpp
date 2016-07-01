#include "compass_search.h"

namespace Optimization {
    namespace Optimizers {

        CompassSearch::CompassSearch(Utilities::Settings::Optimizer *settings, Case *base_case,
                                     Model::Properties::VariablePropertyContainer *variables,
                                     Reservoir::Grid::Grid *grid)
                : Optimizer(settings, base_case, variables, grid)
        {
            step_length_ = settings->parameters().initial_step_length;
            minimum_step_length_ = settings->parameters().minimum_step_length;
        }

        void CompassSearch::step()
        {
            applyNewTentativeBestCase();
        }

        void CompassSearch::contract()
        {
            step_length_ = step_length_/2.0;
        }

        void CompassSearch::perturb()
        {
            QList<Case *> perturbations = QList<Case *>();
            for (QUuid id : tentative_best_case_->integer_variables().keys())
                perturbations.append(tentative_best_case_->Perturb(id, Case::SIGN::PLUSMINUS, step_length_));
            for (QUuid id : tentative_best_case_->real_variables().keys())
                perturbations.append(tentative_best_case_->Perturb(id, Case::SIGN::PLUSMINUS, step_length_));
            for (Case *c : perturbations) {
                constraint_handler_->SnapCaseToConstraints(c);
            }
            case_handler_->AddNewCases(perturbations);
        }

        bool CompassSearch::IsFinished()
        {
            return case_handler_->EvaluatedCases().size() >= max_evaluations_ || step_length_ < minimum_step_length_;
        }

        void CompassSearch::iterate()
        {
            if (iteration_ == 0) {
                perturb();
            }
            else if (betterCaseFoundLastEvaluation()) {
                step();
                perturb();
            }
            else {
                contract();
                perturb();
            }
            case_handler_->ClearRecentlyEvaluatedCases();
        }

        QString CompassSearch::GetStatusStringHeader() const
        {
            return QString("%1,%2,%3,%4,%5,%6,%7")
                    .arg("Iteration")
                    .arg("EvaluatedCases")
                    .arg("QueuedCases")
                    .arg("RecentlyEvaluatedCases")
                    .arg("TentativeBestCaseID")
                    .arg("TentativeBestCaseOFValue")
                    .arg("StepLength");
        }

        QString CompassSearch::GetStatusString() const
        {
            return QString("%1,%2,%3,%4,%5,%6,%7")
                    .arg(iteration_)
                    .arg(nr_evaluated_cases())
                    .arg(nr_queued_cases())
                    .arg(nr_recently_evaluated_cases())
                    .arg(tentative_best_case_->id().toString())
                    .arg(tentative_best_case_->objective_function_value())
                    .arg(step_length_);
        }

    }}
