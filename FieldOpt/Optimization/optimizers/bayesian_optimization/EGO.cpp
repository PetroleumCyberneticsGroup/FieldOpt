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
#include "libgp/include/rprop.h"
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
    VectorXd lb, ub;
    if (constraint_handler_->HasBoundaryConstraints()) {
        auto lb = constraint_handler_->GetLowerBounds(base_case->GetRealVarIdVector());
        auto ub = constraint_handler_->GetUpperBounds(base_case->GetRealVarIdVector());
    }
    else {
        lb.resize(base_case->GetRealVarIdVector().size());
        ub.resize(base_case->GetRealVarIdVector().size());
        lb.fill(settings->parameters().lower_bound);
        ub.fill(settings->parameters().upper_bound);
    }
    n_initial_guesses_ = variables->ContinousVariableSize() * 2;
    af_ = AcquisitionFunction(settings->parameters());
    af_opt_ = AFOptimizers::AFPSO(lb, ub);
    gp_ = new libgp::GaussianProcess(variables->ContinousVariableSize(), "CovMatern5iso");

    // Guess some random initial positions
    auto rng = get_random_generator();
    for (int i = 0; i < n_initial_guesses_; ++i) {
        VectorXd pos = VectorXd::Zero(lb.size());
        for (int i = 0; i < lb.size(); ++i) {
            pos(i) = random_double(rng, lb(i), ub(i));
        }
        Case * init_case = new Case(base_case);
        init_case->SetRealVarValues(pos);
        case_handler_->AddNewCase(init_case);
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
    if (!normalizer_ofv_.is_ready())
        initializeNormalizers();

    gp_->add_pattern(c->GetRealVarVector().data(), normalizer_ofv_.normalize(c->objective_function_value()));
    if (isImprovement(c)) {
        updateTentativeBestCase(c);
        cout << "Found new tent. best: " << c->objective_function_value() << endl;
    }
}
void EGO::iterate() {
    if (!normalizer_ofv_.is_ready())
        initializeNormalizers();

    Eigen::VectorXd params(2);
    params << -1, -1;
    gp_->covf().set_loghyper(params);

    libgp::RProp rprop;
    rprop.init();
    rprop.maximize(gp_, 50, 0);

    VectorXd new_position = af_opt_.Optimize(gp_, af_, normalizer_ofv_.normalize(
        GetTentativeBestCase()->objective_function_value())
    );
    Case *new_case = new Case(case_handler_->AllCases()[0]);
    new_case->SetRealVarValues(new_position);
    case_handler_->AddNewCase(new_case);
    iteration_++;
}
}
}
}
