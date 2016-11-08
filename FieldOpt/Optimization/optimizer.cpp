#include "optimizer.h"

namespace Optimization {

    Optimizer::Optimizer(Settings::Optimizer *settings, Case *base_case,
                         Model::Properties::VariablePropertyContainer *variables,
                         Reservoir::Grid::Grid *grid)
    {
        // Verify that the base case has been evaluated.
        try {
            base_case->objective_function_value();
        } catch (ObjectiveFunctionException) {
            throw OptimizerInitializationException("The objective function value of the base case must be set before initializing an Optimizer.");
        }

        max_evaluations_ = settings->parameters().max_evaluations;
        tentative_best_case_ = base_case;
        case_handler_ = new CaseHandler(tentative_best_case_);
        constraint_handler_ = new Constraints::ConstraintHandler(settings->constraints(), variables, grid);
        iteration_ = 0;
        mode_ = settings->mode();
    }

    bool Optimizer::betterCaseFoundLastEvaluation()
    {
        for (Case* c : case_handler_->RecentlyEvaluatedCases()) {
            if (mode_ == Settings::Optimizer::OptimizerMode::Maximize) {
                if (c->objective_function_value() > tentative_best_case_->objective_function_value())
                    return true;
            }
            else if (mode_ == Settings::Optimizer::OptimizerMode::Minimize) {
                if (c->objective_function_value() < tentative_best_case_->objective_function_value())
                    return true;
            }
        }
        return false;
    }

    void Optimizer::applyNewTentativeBestCase()
    {
        for (Case* c : case_handler_->RecentlyEvaluatedCases()) {
            if (mode_ == Settings::Optimizer::OptimizerMode::Maximize) {
                if (c->objective_function_value() > tentative_best_case_->objective_function_value())
                    tentative_best_case_ = c;
            }
            else if (mode_ == Settings::Optimizer::OptimizerMode::Minimize) {
                if (c->objective_function_value() < tentative_best_case_->objective_function_value())
                    tentative_best_case_ = c;
            }
        }
    }

    Case *Optimizer::GetCaseForEvaluation()
    {
        if (case_handler_->QueuedCases().size() == 0) {
            iterate();
            iteration_++;
        }
        return case_handler_->GetNextCaseForEvaluation();
    }

    void Optimizer::SubmitEvaluatedCase(Case *c)
    {
        case_handler_->UpdateCaseObjectiveFunctionValue(c->id(), c->objective_function_value());
        case_handler_->SetCaseEvaluated(c->id());
    }

    Case *Optimizer::GetTentativeBestCase() const {
        return tentative_best_case_;
    }

    QString Optimizer::GetStatusStringHeader() const
    {
        return QString("%1,%2,%3,%4,%5,%6\n")
                .arg("Iteration")
                .arg("EvaluatedCases")
                .arg("QueuedCases")
                .arg("RecentlyEvaluatedCases")
                .arg("TentativeBestCaseID")
                .arg("TentativeBestCaseOFValue");
    }

    QString Optimizer::GetStatusString() const
    {
        return QString("%1,%2,%3,%4,%5,%6\n")
                .arg(iteration_)
                .arg(nr_evaluated_cases())
                .arg(nr_queued_cases())
                .arg(nr_recently_evaluated_cases())
                .arg(tentative_best_case_->id().toString())
                .arg(tentative_best_case_->objective_function_value());
    }

    void Optimizer::EnableConstraintLogging(QString output_directory_path) {
        for (Constraints::Constraint *con : constraint_handler_->constraints())
            con->EnableLogging(output_directory_path);
    }

    void Optimizer::SetVerbosityLevel(int level) {
        verbosity_level_ = level;
        for (auto con : constraint_handler_->constraints())
            con->SetVerbosityLevel(level);
    }


}

