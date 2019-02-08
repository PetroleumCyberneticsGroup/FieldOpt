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
#include <Utilities/time.hpp>
#include "optimizer.h"
#include <time.h>
#include <cmath>

namespace Optimization {

Optimizer::Optimizer(Settings::Optimizer *settings, Case *base_case,
                     Model::Properties::VariablePropertyContainer *variables,
                     Reservoir::Grid::Grid *grid,
                     Logger *logger,
                     CaseHandler *case_handler,
                     Constraints::ConstraintHandler *constraint_handler
)
{
    // Verify that the base case has been evaluated.
    try {
        base_case->objective_function_value();
    } catch (ObjectiveFunctionException) {
        throw OptimizerInitializationException("The objective function value of the base case must be set before initializing an Optimizer.");
    }

    max_evaluations_ = settings->parameters().max_evaluations;
    tentative_best_case_ = base_case;
    tentative_best_case_iteration_ = 0;
    if (case_handler == 0) {
        case_handler_ = new CaseHandler(tentative_best_case_);
    }
    else {
        Printer::ext_info("Using shared CaseHandler.", "Optimizer", "Optimization");
        case_handler_ = case_handler;
    }
    if (constraint_handler == 0) {
        constraint_handler_ = new Constraints::ConstraintHandler(settings->constraints(), variables, grid);
    }
    else {
        Printer::ext_info("Using shared ConstraintHandler.", "Optimizer", "Optimization");
        constraint_handler_ = constraint_handler;
    }
    iteration_ = 0;
    evaluated_cases_ = 0;
    mode_ = settings->mode();
    is_async_ = false;
    start_time_ = QDateTime::currentDateTime();
    logger_ = logger;
    enable_logging_ = true;
    verbosity_level_ = 0;
    penalize_ = settings->objective().use_penalty_function;
}

Case *Optimizer::GetCaseForEvaluation()
{
    if (case_handler_->QueuedCases().size() == 0) {
        time_t start, end;
        time(&start);
        iterate();
        time(&end);
        seconds_spent_in_iterate_ = difftime(end, start);
    }
    return case_handler_->GetNextCaseForEvaluation();
}

void Optimizer::SubmitEvaluatedCase(Case *c)
{
    evaluated_cases_++;
    if (penalize_ && iteration_ > 0) {
        double penalized_ofv = PenalizedOFV(c);
        c->set_objective_function_value(penalized_ofv);
    }
    case_handler_->UpdateCaseObjectiveFunctionValue(c->id(), c->objective_function_value());
    case_handler_->SetCaseState(c->id(), c->state, c->GetWICTime(), c->GetSimTime());
    case_handler_->SetCaseEvaluated(c->id());
    handleEvaluatedCase(case_handler_->GetCase(c->id()));
    if (enable_logging_) {
        logger_->AddEntry(case_handler_->GetCase(c->id()));
    }
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
int Optimizer::GetSimulationDuration(Case *c) {
    auto cs = case_handler_->GetCase(c->id());
    if (cs->state.eval != Case::CaseState::EvalStatus::E_DONE) {
        return -1;
    }
    return c->GetSimTime();
}
Loggable::LogTarget Optimizer::GetLogTarget() {
    return Loggable::LogTarget::LOG_OPTIMIZER;
}
map<string, string> Optimizer::GetState() {
    return map<string, string>();
}
QUuid Optimizer::GetId() {
    return tentative_best_case_->GetId();
}
map<string, vector<double>> Optimizer::GetValues() {
    map<string, vector<double>> valmap;
    valmap["TimeEl"] = vector<double>{time_since_seconds(start_time_)};
    valmap["IterNr"] = vector<double>{iteration_};
    valmap["TimeIt"] = vector<double>{seconds_spent_in_iterate_};
    valmap["TotlNr"] = vector<double>{case_handler_->NumberTotal()};
    valmap["EvalNr"] = vector<double>{case_handler_->NumberSimulated()};
    valmap["BkpdNr"] = vector<double>{case_handler_->NumberBookkeeped()};
    valmap["TimONr"] = vector<double>{case_handler_->NumberTimeout()};
    valmap["FailNr"] = vector<double>{case_handler_->NumberFailed()};
    valmap["InvlNr"] = vector<double>{case_handler_->NumberInvalid()};
    valmap["CBOFnV"] = vector<double>{tentative_best_case_->objective_function_value()};
    return valmap;
}

Loggable::LogTarget Optimizer::Summary::GetLogTarget() {
    return LOG_SUMMARY;
}
map<string, string> Optimizer::Summary::GetState() {
    map<string, string> statemap  = ext_state_;
    statemap["Start"] = timestamp_string(opt_->start_time_);
    statemap["Duration"] = timespan_string(
        time_span_seconds(opt_->start_time_, QDateTime::currentDateTime())
    );
    statemap["End"] = timestamp_string(QDateTime::currentDateTime());
    switch (cond_) {
        case MAX_EVALS_REACHED: statemap["Term. condition"] = "Reached max. sims"; break;
        case MINIMUM_STEP_LENGTH_REACHED: statemap["Term. condition"] = "Reached min. step length"; break;
        case MAX_ITERATIONS_REACHED: statemap["Term. condition"] = "Reached max. iterations"; break;
        default: statemap["Term. condition"] = "Unknown";
    }
    statemap["bc Best case found in iter"] = boost::lexical_cast<string>(opt_->tentative_best_case_iteration_);
    statemap["bc UUID"] = opt_->tentative_best_case_->GetId().toString().toStdString();
    statemap["bc Objective function value"] = boost::lexical_cast<string>(opt_->tentative_best_case_->objective_function_value());
    statemap["bc Constraint status"] = statemap["bc Constraint status"] = opt_->tentative_best_case_->GetState()["ConsSt"];
    statemap["bc Simulation time"] = timespan_string(opt_->tentative_best_case_->GetSimTime());
    return statemap;
}
QUuid Optimizer::Summary::GetId() {
    return opt_->tentative_best_case_->GetId();
}
map<string, vector<double>> Optimizer::Summary::GetValues() {
    map<string, vector<double>> valmap;
    valmap["generated"] = vector<double>{opt_->case_handler_->NumberTotal()};
    valmap["simulated"] = vector<double>{opt_->case_handler_->NumberSimulated()};
    valmap["invalid"] = vector<double>{opt_->case_handler_->NumberInvalid()};
    valmap["failed"] = vector<double>{opt_->case_handler_->NumberFailed()};
    valmap["timed out"] = vector<double>{opt_->case_handler_->NumberTimeout()};
    valmap["bookkeeped"] = vector<double>{opt_->case_handler_->NumberBookkeeped()};
    return valmap;
}

void Optimizer::updateTentativeBestCase(Case *c) {
    tentative_best_case_ = c;
    tentative_best_case_iteration_ = iteration_;
}

void Optimizer::initializeNormalizers() {
    initializeOfvNormalizer();
    constraint_handler_->InitializeNormalizers(case_handler_->AllCases());
}

void Optimizer::initializeOfvNormalizer() {
    if (case_handler_->EvaluatedCases().size() == 0 || normalizer_ofv_.is_ready())
        throw runtime_error("Unable to initialize normalizer with no evaluated cases available.");

    vector<double> abs_ofvs;
    for (auto c : case_handler_->EvaluatedCases()) {
        abs_ofvs.push_back(abs(c->objective_function_value()));
    }
    long double max_ofv = *max_element(abs_ofvs.begin(), abs_ofvs.end());

    normalizer_ofv_.set_max(1.0L);
    normalizer_ofv_.set_midpoint(max_ofv);
    normalizer_ofv_.set_steepness(1.0L / max_ofv);
}

double Optimizer::PenalizedOFV(Case *c) {
    long double norm_ofv = normalizer_ofv_.normalize(c->objective_function_value());
    long double penalty = constraint_handler_->GetWeightedNormalizedPenalties(c);
    long double norm_pen_ovf = norm_ofv - penalty;

    if (norm_pen_ovf <= 0.0L) {
        cout << "RETURNING ZERO OFV" << endl;
        return 0.0;
    }
    else {
        return normalizer_ofv_.denormalize(norm_pen_ovf);
    }
}

void Optimizer::DisableLogging() {
    enable_logging_ = false;
}
}

