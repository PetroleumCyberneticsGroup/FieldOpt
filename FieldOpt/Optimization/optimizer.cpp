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
    is_async_ = false;
}

Case *Optimizer::GetCaseForEvaluation()
{
    if (case_handler_->QueuedCases().size() == 0) {
        iterate();
    }
    return case_handler_->GetNextCaseForEvaluation();
}

void Optimizer::SubmitEvaluatedCase(Case *c)
{
    case_handler_->UpdateCaseObjectiveFunctionValue(c->id(), c->objective_function_value());
    case_handler_->SetCaseEvaluated(c->id());
    case_handler_->SetCaseEvalStatus(c->id(), c->state.eval);
    case_handler_->SetCaseErrMsg(c->id(), c->state.err_msg);
    handleEvaluatedCase(case_handler_->GetCase(c->id()));
}

Case *Optimizer::GetTentativeBestCase() const {
    return tentative_best_case_;
}

bool Optimizer::isImprovement(const Case *c) {
    return isBetter(c, tentative_best_case_);
}

bool Optimizer::isBetter(const Case *c1, const Case *c2) const {
    if (mode_ == Settings::Optimizer::OptimizerMode::Maximize) {
        if (c1->objective_function_value() > c2->objective_function_value())
            return true;
    }
    else if (mode_ == Settings::Optimizer::OptimizerMode::Minimize) {
        if (c1->objective_function_value() < c2->objective_function_value())
            return true;
    }
    return false;
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

