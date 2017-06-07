/******************************************************************************
   Created by einar on 6/7/17.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "Utilities/math.hpp"
#include "optimizers/bayesian_optimization/af_optimizers/AFPSO.h"
#include "EGO.h"

namespace Optimization {
namespace Optimizers {
namespace BayesianOptimization {

EGO::EGO(Settings::Optimizer *settings,
         Case *base_case,
         Model::Properties::VariablePropertyContainer *variables,
         Reservoir::Grid::Grid *grid,
         Logger *logger) : Optimizer(settings, base_case, variables, grid, logger) {
    auto lb = constraint_handler_->GetLowerBounds(base_case->GetRealVarIdVector());
    auto ub = constraint_handler_->GetUpperBounds(base_case->GetRealVarIdVector());
    n_initial_guesses_ = variables->ContinousVariableSize() * 2;
    af_ = AcquisitionFunction(settings->parameters());
    af_opt_ = AFOptimizers::AFPSO(lb, ub);
    gp_ = new libgp::GaussianProcess(variables->ContinousVariableSize(), "CovSum ( CovSEiso, CovNoise)");

    // Guess some random initial positions
    auto rng = get_random_generator();
    for (int i = 0; i < n_initial_guesses_; ++i) {
        VectorXd pos = VectorXd::Zero(lb.size());
        for (int i = 0; i < lb.size(); ++i) {
            pos(i) = random_double(rng, lb(i), ub(i));
        }
    }
}
Optimization::Optimizer::TerminationCondition EGO::IsFinished() {
    TerminationCondition tc = NOT_FINISHED;
    if (case_handler_->CasesBeingEvaluated().size() > 0)
        return tc;
    if (case_handler_->NumberSimulated() > max_evaluations_)
        tc = MAX_EVALS_REACHED;
    if (tc != NOT_FINISHED) {
        logger_->AddEntry(this);
        logger_->AddEntry(new Summary(this, tc));
    }
    return tc;
}
void EGO::handleEvaluatedCase(Case *c) {
    gp_->add_pattern(c->GetRealVarVector().data(), c->objective_function_value());
}
void EGO::iterate() {
    VectorXd new_position = af_opt_.Optimize(gp_, af_);
    Case *new_case = new Case(case_handler_->AllCases()[0]);
    new_case->SetRealVarValues(new_position);
    case_handler_->AddNewCase(new_case);
}
}
}
}
