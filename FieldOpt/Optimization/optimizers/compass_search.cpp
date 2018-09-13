#include <iostream>
#include "compass_search.h"
#include "gss_patterns.hpp"
#include "Utilities/verbosity.h"
#include "Utilities/printer.hpp"
#include "Utilities/stringhelpers.hpp"
namespace Optimization {
    namespace Optimizers {

        CompassSearch::CompassSearch(Settings::Optimizer *settings,
                                     Case *base_case,
                                     Model::Properties::VariablePropertyContainer *variables,
                                     Reservoir::Grid::Grid *grid,
                                     Logger *logger,
                                     CaseHandler *case_handler,
                                     Constraints::ConstraintHandler *constraint_handler
        )
                : GSS(settings, base_case, variables, grid, logger, case_handler, constraint_handler)
        {
        }

        void CompassSearch::iterate()
        {
            if (VERB_OPT >= 2) {
                Printer::ext_info("Compass search iteration " + Printer::num2str(iteration_ + ".|")
                    + "Step lengths: " + eigenvec_to_str(step_lengths_));
            }
            if (enable_logging_) {
                logger_->AddEntry(this);
            }
            if (!is_successful_iteration() && iteration_ != 0)
                contract();
            case_handler_->AddNewCases(generate_trial_points());
            case_handler_->ClearRecentlyEvaluatedCases();
            iteration_++;
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
                    .arg(GetTentativeBestCase()->id().toString())
                    .arg(GetTentativeBestCase()->objective_function_value())
                    .arg(step_lengths_[0]);
        }

        void CompassSearch::handleEvaluatedCase(Case *c) {
            if (isImprovement(c))
                updateTentativeBestCase(c);
        }

        bool CompassSearch::is_successful_iteration() {
            return case_handler_->RecentlyEvaluatedCases().contains(GetTentativeBestCase());
        }

    }}
