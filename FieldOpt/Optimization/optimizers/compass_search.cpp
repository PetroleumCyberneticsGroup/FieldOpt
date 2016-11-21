#include <iostream>
#include "compass_search.h"
#include "gss_patterns.hpp"

namespace Optimization {
    namespace Optimizers {

        CompassSearch::CompassSearch(Settings::Optimizer *settings, Case *base_case,
                                     Model::Properties::VariablePropertyContainer *variables,
                                     Reservoir::Grid::Grid *grid)
                : GSS(settings, base_case, variables, grid)
        {
            directions_ = GSSPatterns::Compass(num_vars_);
            step_lengths_ = Eigen::VectorXd(directions_.size());
            step_lengths_.fill(settings->parameters().initial_step_length);
        }

        void CompassSearch::iterate()
        {
            if (!is_successful_iteration() && iteration_ != 0)
                contract();
            case_handler_->AddNewCases(generate_trial_points());
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
                    .arg(step_lengths_[0]);
        }

        void CompassSearch::handleEvaluatedCase(Case *c) {
            if (isImprovement(c))
                tentative_best_case_ = c;
        }

        bool CompassSearch::is_successful_iteration() {
            return case_handler_->RecentlyEvaluatedCases().contains(tentative_best_case_);
        }

    }}
