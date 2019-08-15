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
#include <Utilities/verbosity.h>
#include "Utilities/printer.hpp"
#include "Utilities/stringhelpers.hpp"
#include "gp/rprop.h"
#include "Utilities/math.hpp"
#include "Utilities/random.hpp"
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
         Logger *logger,
         CaseHandler *case_handler,
         Constraints::ConstraintHandler *constraint_handler
) : Optimizer(settings, base_case, variables, grid, logger, case_handler, constraint_handler) {

    time_fitting_ = 0;
    time_af_opt_ = 0;
    settings_ = settings;

    initializeNormalizers();
    
    // penalize the base case
    if (penalize_) {
        double org_ofv = tentative_best_case_->objective_function_value();
        double pen_ofv = PenalizedOFV(tentative_best_case_);
        tentative_best_case_->set_objective_function_value(pen_ofv);
        if (VERB_OPT >=1) {
            Printer::ext_info("Penalized base case. " 
                    "Original value: " + Printer::num2str(org_ofv) + "; "
                    + "Penalized value: " + Printer::num2str(pen_ofv), "Optimization", "Optimizer");
        }
    }

    int n_cont_vars = variables->ContinousVariableSize();

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

    if (enable_logging_) { // Log base case
        logger_->AddEntry(this);
    }

    af_ = AcquisitionFunction(settings->parameters());
    af_opt_ = AFOptimizers::AFPSO(lb_, ub_, settings->parameters().rng_seed);
    gp_ = new libgp::GaussianProcess(n_cont_vars, settings->parameters().ego_kernel);


    if (settings->parameters().ego_init_sampling_method == "Random") {
        if (settings->parameters().ego_init_guesses == -1) {
            n_initial_guesses_ = n_cont_vars * 2;
        }
        else {
            n_initial_guesses_ = settings->parameters().ego_init_guesses;
        }
        auto rng = get_random_generator(settings->parameters().rng_seed);
        for (int i = 0; i < n_initial_guesses_; ++i) {
            VectorXd pos = VectorXd::Zero(lb_.size());
            for (int i = 0; i < lb_.size(); ++i) {
                pos(i) = random_double(rng, lb_(i), ub_(i));
            }
            Case * init_case = new Case(base_case);
            init_case->SetRealVarValues(pos);
            case_handler_->AddNewCase(init_case);
        }
    }
    else if (settings->parameters().ego_init_sampling_method == "Uniform") {
        // Step half-way from initial guess to bounds on each axis
        // and all the way to bounds.
        auto init_values = base_case->GetRealVarVector();
        for (int i = 0; i < init_values.size(); ++i) {
            vector<double> values;
            values.push_back(init_values[i] + abs(ub_[i] - init_values[i]) / 2.0);
            values.push_back(init_values[i] - abs(init_values[i] - lb_[i]) / 2.0);
            values.push_back(ub_[i]);
            values.push_back(lb_[i]);

            for (double value : values) {
                Case * pert_case = new Case(base_case);
                auto pert_vec = init_values;
                pert_vec[i] = value;
                pert_case->SetRealVarValues(pert_vec);
                case_handler_->AddNewCase(pert_case);
            }
        }
    }
    else {
        Printer::ext_warn("Only the Random and Uniform sampling methods are implemented.", "Optimization", "EGO");
        throw std::runtime_error("Failed to initialize EGO optimizer.");
    }

    // Initialize GP hyperparameters
    std::map<std::string, int> map_kernel_to_n_hyper = {
        { "CovLinearard",          n_cont_vars   },
        { "CovLinearone",          1             },
        { "CovMatern3iso",         2             },
        { "CovMatern5iso",         2             },
        { "CovNoise",              1             },
        { "CovRQiso",              3             },
        { "CovSEard",              n_cont_vars+1 },
        { "CovSEiso",              2             },
        { "CovPeriodicMatern3iso", 3             },
        { "CovPeriodic",           3             }
    };
    Eigen::VectorXd params(map_kernel_to_n_hyper[settings->parameters().ego_kernel]);
    params.fill(-1);
    gp_->covf().set_loghyper(params);


    if (enable_logging_) {
        logger_->AddEntry(new ConfigurationSummary(this));
    }
}
Optimization::Optimizer::TerminationCondition EGO::IsFinished() {
    TerminationCondition tc = NOT_FINISHED;
    if (case_handler_->CasesBeingEvaluated().size() > 0)
        return tc;
    if (evaluated_cases_ > max_evaluations_)
        tc = MAX_EVALS_REACHED;
    if (tc != NOT_FINISHED) {
        map<string, string> ext_state;
        ext_state["Time in AF opt"] = boost::lexical_cast<string>(time_af_opt_);
        ext_state["Time in GP opt"] = boost::lexical_cast<string>(time_fitting_);
        if (enable_logging_) {
            logger_->AddEntry(this);
            logger_->AddEntry(new Summary(this, tc, ext_state));
        }
    }
    return tc;
}
void EGO::handleEvaluatedCase(Case *c) {
    gp_->add_pattern(c->GetRealVarVector().data(), normalizer_ofv_.normalize(c->objective_function_value()));
    if (isImprovement(c)) {
        updateTentativeBestCase(c);
        Printer::ext_info("Found new tentative best case: " + Printer::num2str(c->objective_function_value()), "Optimization", "EGO");
    }
}
void EGO::iterate() {
    if (enable_logging_) {
        logger_->AddEntry(this);
    }

    // Optimize GP hyperparameters
    QDateTime start, end;
    start = QDateTime::currentDateTime();
    libgp::RProp rprop;
    rprop.init();
    if (VERB_OPT >= 3) {
        Printer::ext_info("Optimizing Gaussian Process kernel hyperparameters ... ", "Optimization", "EGO");
        rprop.maximize(gp_, 100, 1);
    }
    else {
        rprop.maximize(gp_, 100, 0);
    }
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
    statemap["Kernel"] = opt_->settings_->parameters().ego_kernel;
    statemap["Acquisition function"] = opt_->settings_->parameters().ego_af;
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
