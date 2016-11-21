#include <iostream>
#include "compass_search.h"

namespace Optimization {
    namespace Optimizers {

        CompassSearch::CompassSearch(Settings::Optimizer *settings, Case *base_case,
                                     Model::Properties::VariablePropertyContainer *variables,
                                     Reservoir::Grid::Grid *grid)
                : GSS(settings, base_case, variables, grid)
        {
            int numRvars = base_case->GetRealVarVector().size();
            int numIvars = base_case->GetIntegerVarVector().size();
            int num_vars = numRvars + numIvars;
            if (numRvars > 0 && numIvars > 0)
                std::cout << ("WARNING: Compass search does not handle both continuous and discrete variables at the same time");

            // Generate set of direction vectors (all coordinate directions)
            directions_ = std::vector<Eigen::VectorXi>(2*num_vars);
            for (int i = 0; i < num_vars; ++i) {
                Eigen::VectorXi dir = Eigen::VectorXi::Zero(num_vars);
                dir(i) = 1;
                directions_[i] = dir;
                directions_[i+num_vars] = (-1) * dir;
            }

            // Generate list of step lengths
            step_lengths_ = Eigen::VectorXd(2*num_vars);
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
