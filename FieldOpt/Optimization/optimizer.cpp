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
        is_hybrid_component_ = true;
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

    if (penalize_) {
        if (!normalizer_ofv_.is_ready()) {
            if (VERB_OPT >=1) {
                Printer::ext_info("Initializing normalizers", "Optimization", "Optimizer");
            }
            initializeNormalizers();
            
            // penalize the base case
            double org_ofv = tentative_best_case_->objective_function_value();
            double pen_ofv = PenalizedOFV(tentative_best_case_);
            tentative_best_case_->set_objective_function_value(pen_ofv);
            if (VERB_OPT >=1) {
                Printer::ext_info("Penalized base case. " 
                        "Original value: " + Printer::num2str(org_ofv) + "; "
                        + "Penalized value: " + Printer::num2str(pen_ofv), "Optimization", "Optimizer");
            }
        }
    }
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
    if (penalize_) {
        double penalized_ofv = PenalizedOFV(c);
        c->set_objective_function_value(penalized_ofv);
    }
    case_handler_->UpdateCaseObjectiveFunctionValue(c->id(), c->objective_function_value());
    if (c->mpso_id_ofv().size() > 0) {
        case_handler_->UpdateCase_mpso_id_ofv(c->id(), c->mpso_id_ofv());
    }
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
    map<string, string> statemap;

    QUuid best_case_id = tentative_best_case_->GetId();
    int best_case_iteration = tentative_best_case_iteration_;
    statemap["BC_id"] = best_case_id.toString().toStdString();
    statemap["BC_iteration"] = to_string(best_case_iteration);
    for (auto key : tentative_best_case_->real_variables().keys()){
        std::string var_name = tentative_best_case_->variables_name().value(key);
        double var_value = tentative_best_case_->real_variables().value(key);
        statemap["BC_Var#"+var_name] = QString::number(var_value, 'f', 16).toStdString();
    }
    if (tentative_best_case_->real_variables_velocity().size() > 0) {
        for (auto key : tentative_best_case_->real_variables_velocity().keys()) {
            std::string var_name = tentative_best_case_->variables_name().value(key);
            double var_velocity = tentative_best_case_->real_variables_velocity().value(key);
            statemap["BC_VarVelocity#"+var_name] = QString::number(var_velocity, 'f', 16).toStdString();
        }
    }

    if (mpso_id_r_CO2().size() > 0) {
        QHash<QUuid, double> mpso_id_r_CO2_ = mpso_id_r_CO2();
        QHash<QUuid, Case *> mpso_id_tentative_best_case_ = mpso_id_tentative_best_case();
        QHash<QUuid, int> mpso_id_tentative_best_case_iteration_ = mpso_id_tentative_best_case_iteration();

        QList<QUuid> ObjFn_ids = mpso_id_r_CO2_.keys();
        for (int i = 0; i < mpso_id_r_CO2_.size(); ++i) {
            QUuid ObjFn_id = ObjFn_ids[i];

            double mpso_r_CO2 = mpso_id_r_CO2_.value(ObjFn_id);
            QUuid mpso_best_case_id = mpso_id_tentative_best_case_.value(ObjFn_id)->GetId();
            int mpso_bast_case_iteration = mpso_id_tentative_best_case_iteration_.value(ObjFn_id);
            double mpso_best_case_ObjFn_value = mpso_id_tentative_best_case_.value(ObjFn_id)->mpso_id_ofv().value(ObjFn_id);
            QUuid mpso_best_case_ObjFn_id = mpso_id_tentative_best_case_.value(ObjFn_id)->ObjFn_id();
            double mpso_best_case_r_CO2 = mpso_id_r_CO2_.value(mpso_best_case_ObjFn_id);
            statemap["mpso_r_CO2_" + to_string(i)] = QString::number(mpso_r_CO2, 'f', 1).toStdString();
            statemap["mpso_BC_id_" + to_string(i)] = mpso_best_case_id.toString().toStdString();
            statemap["mpso_BC_iteration_" + to_string(i)] = to_string(mpso_bast_case_iteration);
            statemap["mpso_BC_r_CO2_" + to_string(i)] = QString::number(mpso_best_case_r_CO2, 'f', 1).toStdString();
            statemap["mpso_BC_ObjFn_value_" + to_string(i)] = QString::number(mpso_best_case_ObjFn_value, 'f', 2).toStdString();
            for (auto key : mpso_id_tentative_best_case_.value(ObjFn_id)->real_variables().keys()){
                std::string var_name = mpso_id_tentative_best_case_.value(ObjFn_id)->variables_name().value(key);
                double var_value = mpso_id_tentative_best_case_.value(ObjFn_id)->real_variables().value(key);
                statemap["mpso_BC_" + to_string(i) + "_Var#" + var_name] = QString::number(var_value, 'f', 16).toStdString();
            }
            if (mpso_id_tentative_best_case_.value(ObjFn_id)->real_variables_velocity().size() > 0) {
                for (auto key : mpso_id_tentative_best_case_.value(ObjFn_id)->real_variables_velocity().keys()) {
                    std::string var_name = mpso_id_tentative_best_case_.value(ObjFn_id)->variables_name().value(key);
                    double var_velocity = mpso_id_tentative_best_case_.value(ObjFn_id)->real_variables_velocity().value(key);
                    statemap["mpso_BC_" + to_string(i) + "_VarVelocity#" + var_name] = QString::number(var_velocity, 'f', 16).toStdString();
                }
            }
        }
    }
    return statemap;
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
    if (mpso_id_r_CO2().size() > 0) {
        valmap["MPSO-NumberOfSwarms"] = vector<double>{mpso_id_r_CO2().size()};
    }
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
    double denormalized_ofv = normalizer_ofv_.denormalize(norm_pen_ovf);

    if (VERB_OPT >= 2) {
        Printer::ext_info("Penalized case "     + c->id().toString().toStdString()                + ". "
                "Initial OFV: "                 + Printer::num2str(c->objective_function_value()) + "; "
                "Normalized OFV :"              + Printer::num2str(norm_ofv)                      + "; "
                "Normalized penalty: "          + Printer::num2str(penalty)                       + "; "
                "Penalized, normalized OFV: "   + Printer::num2str(norm_pen_ovf)                  + "; "
                "Denormalized, penalized OFV: " + Printer::num2str(denormalized_ofv), "Optimization", "Optimizer");
    }

    if (norm_pen_ovf <= 0.0L) {
        cout << "RETURNING ZERO OFV" << endl;
        return 0.0;
    }
    else {
        return denormalized_ofv;
    }
}

void Optimizer::DisableLogging() {
    enable_logging_ = false;
}
}

