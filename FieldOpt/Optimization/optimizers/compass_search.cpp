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

#include "compass_search.h"

namespace Optimization { namespace Optimizers {

CompassSearch::CompassSearch(Utilities::Settings::Optimizer *settings, Case *base_case, Model::Properties::VariablePropertyContainer *variables)
    : Optimizer(settings, base_case, variables)
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
    foreach (QUuid id, tentative_best_case_->integer_variables().keys())
        perturbations.append(tentative_best_case_->Perturb(id, Case::SIGN::PLUSMINUS, step_length_));
    foreach (QUuid id, tentative_best_case_->real_variables().keys())
        perturbations.append(tentative_best_case_->Perturb(id, Case::SIGN::PLUSMINUS, step_length_));
    foreach (Case *c, perturbations) {
        constraint_handler_->SnapCaseToConstraints(c);
    }
    case_handler_->AddNewCases(perturbations);
}

bool CompassSearch::IsFinished()
{
    if (case_handler_->EvaluatedCases().size() >= max_evaluations_ || step_length_ < minimum_step_length_)
        return true;
    else return false;
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
