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
#include "Utilities/stringhelpers.hpp"
#include "gp/rprop.h"
#include "Utilities/math.hpp"
#include "Utilities/time.hpp"
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
    if (constraint_handler_->HasBoundaryConstraints()) {
        lb_ = constraint_handler_->GetLowerBounds(base_case->GetRealVarIdVector());
        ub_ = constraint_handler_->GetUpperBounds(base_case->GetRealVarIdVector());
    }
    else {
        lb_.resize(base_case->GetRealVarIdVector().size());
        ub_.resize(base_case->GetRealVarIdVector().size());
        lb_.fill(settings->parameters().lower_bound);
        ub_.fill(settings->parameters().upper_bound);
    }
    n_initial_guesses_ = variables->ContinousVariableSize() * 2;
    af_ = AcquisitionFunction(settings->parameters());
    af_opt_ = AFOptimizers::AFPSO(lb_, ub_);
    gp_ = new libgp::GaussianProcess(variables->ContinousVariableSize(), "CovMatern5iso");

    cout << "Lower bounds: " << eigenvec_to_str(lb_) << endl;
    cout << "Upper bounds: " << eigenvec_to_str(ub_) << endl;

    // Guess some random initial positions
    auto rng = get_random_generator();
    for (int i = 0; i < n_initial_guesses_; ++i) {
        VectorXd pos = VectorXd::Zero(lb_.size());
        for (int i = 0; i < lb_.size(); ++i) {
            pos(i) = random_double(rng, lb_(i), ub_(i));
        }
        Case * init_case = new Case(base_case);
        init_case->SetRealVarValues(pos);
        case_handler_->AddNewCase(init_case);
    }

    // Initialize GP hyperparameters
    Eigen::VectorXd params(2);
    params << -1, -1;
    gp_->covf().set_loghyper(params);

    time_fitting_ = 0;
    time_af_opt_ = 0;

    logger_->AddEntry(new ConfigurationSummary(this));
}
Optimization::Optimizer::TerminationCondition EGO::IsFinished() {
    TerminationCondition tc = NOT_FINISHED;
    if (case_handler_->CasesBeingEvaluated().size() > 0)
        return tc;
    if (case_handler_->NumberSimulated() > max_evaluations_)
        tc = MAX_EVALS_REACHED;
    if (tc != NOT_FINISHED) {
        map<string, string> ext_state;
        ext_state["Time in AF opt"] = boost::lexical_cast<string>(time_af_opt_);
        ext_state["Time in GP opt"] = boost::lexical_cast<string>(time_fitting_);
        logger_->AddEntry(this);
        logger_->AddEntry(new Summary(this, tc, ext_state));
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
    logger_->AddEntry(this);
    if (!normalizer_ofv_.is_ready())
        initializeNormalizers();

    // Optimize GP hyperparameters
    libgp::RProp rprop;
    rprop.init();

    QDateTime start, end;

    start = QDateTime::currentDateTime();
    rprop.maximize(gp_, 50, 0);
    end = QDateTime::currentDateTime();
    time_fitting_ += time_span_seconds(start, end);

    start = QDateTime::currentDateTime();
    VectorXd new_position = af_opt_.Optimize(
        gp_, af_,
        normalizer_ofv_.normalize(GetTentativeBestCase()->objective_function_value())
    );
    end = QDateTime::currentDateTime();
    time_af_opt_ += time_span_seconds(start, end);

    for (int i = 0; i < new_position.size(); ++i) {
        if (new_position(i) < lb_(i)) {
            new_position(i) = lb_(i);
            cout << "Snapped to LB." << endl;
        } else if (new_position(i) > ub_(i)) {
            new_position(i) = ub_(i);
            cout << "Snapped to UB." << endl;
        }
    }
    Case *new_case = new Case(case_handler_->AllCases()[0]);
    new_case->SetRealVarValues(new_position);
    case_handler_->AddNewCase(new_case);
    iteration_++;
}

Loggable::LogTarget EGO::ConfigurationSummary::GetLogTarget() {
    return LOG_SUMMARY;
}
map<string, string> EGO::ConfigurationSummary::GetState() {
    map<string, string> statemap;
    statemap["Name"] = "Efficient Global Optimization (EGO)";
    statemap["AF Optimizer"] = "PSO";
    statemap["Mode"] = opt_->mode_ == Settings::Optimizer::OptimizerMode::Maximize ? "Maximize" : "Minimize";
    statemap["Max Evaluations"] = boost::lexical_cast<string>(opt_->max_evaluations_);
    statemap["Num. initial guesses"] = boost::lexical_cast<string>(opt_->n_initial_guesses_);

    string constraints_used = "";
    for (auto cons : opt_->constraint_handler_->constraints()) {
        constraints_used += cons->name() + " ";
    }
    statemap["Constraints"] = constraints_used;
    return statemap;
}
QUuid EGO::ConfigurationSummary::GetId() {
    return QUuid(); // Null UUID
}
map<string, vector<double>> EGO::ConfigurationSummary::GetValues() {
    map<string, vector<double>> valmap;
    return valmap;
}

}
}
}
