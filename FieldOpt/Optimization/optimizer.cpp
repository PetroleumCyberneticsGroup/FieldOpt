/******************************************************************************
 *
 *
 *
 * Created: 04.12.2015 2015 by einar
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

#include "optimizer.h"

namespace Optimization {

Optimizer::Optimizer(Utilities::Settings::Optimizer *settings, Case *base_case, Model::Properties::VariablePropertyContainer *variables)
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
    constraint_handler_ = new Constraints::ConstraintHandler(settings->constraints(), variables);
    iteration_ = 0;
    mode_ = settings->mode();
}

bool Optimizer::betterCaseFoundLastEvaluation()
{
    foreach (Case* c, case_handler_->RecentlyEvaluatedCases()) {
        if (mode_ == Utilities::Settings::Optimizer::OptimizerMode::Maximize) {
            if (c->objective_function_value() > tentative_best_case_->objective_function_value())
                return true;
        }
        else if (mode_ == Utilities::Settings::Optimizer::OptimizerMode::Minimize) {
            if (c->objective_function_value() < tentative_best_case_->objective_function_value())
                return true;
        }
    }
    return false;
}

void Optimizer::applyNewTentativeBestCase()
{
    foreach (Case* c, case_handler_->RecentlyEvaluatedCases()) {
        if (mode_ == Utilities::Settings::Optimizer::OptimizerMode::Maximize) {
            if (c->objective_function_value() > tentative_best_case_->objective_function_value())
                tentative_best_case_ = c;
        }
        else if (mode_ == Utilities::Settings::Optimizer::OptimizerMode::Minimize) {
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

}

