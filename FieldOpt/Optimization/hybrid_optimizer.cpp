/******************************************************************************
   Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include <Optimization/optimizers/compass_search.h>
#include <Optimization/optimizers/APPS.h>
#include <Optimization/optimizers/RGARDD.h>
#include <Optimization/optimizers/bayesian_optimization/EGO.h>
#include <Utilities/printer.hpp>
#include <Utilities/verbosity.h>
#include "hybrid_optimizer.h"

namespace Optimization {

HybridOptimizer::HybridOptimizer(Settings::Optimizer *settings,
         Case *base_case,
         Model::Properties::VariablePropertyContainer *variables,
         Reservoir::Grid::Grid *grid,
         Logger *logger
)
    : Optimizer(settings, base_case, variables, grid, logger)
{
    variables_ = variables;
    grid_ = grid;

    assert(settings->HybridComponents().size() == 2);
    primary_settings_ = new Settings::Optimizer(settings->HybridComponents()[0]);
    secondary_settings_ = new Settings::Optimizer(settings->HybridComponents()[1]);

    initializeComponent(0);
    active_component_ = 0;
    iteration_ = 0;

    component_improvement_found_ = true;

    max_hybrid_iterations_ = settings->parameters().hybrid_max_iterations;
    if (settings->parameters().hybrid_switch_mode == "OnConvergence") {
        switch_mode_ = HybridSwitchMode::ON_CONVERGENCE;
    }
    else {
        throw std::runtime_error("Hybrid optimizer switch mode not recognized.");
    }
    if (settings->parameters().hybrid_termination_condition == "NoImprovement") {
        hybrid_termination_condition_ = HybridTerminationCondition::NO_IMPROVEMENT;
    }
    else {
        throw std::runtime_error("Hybrid optimizer termination condition not recognized.");
    }
}

Optimizer::TerminationCondition HybridOptimizer::IsFinished() {
    if (hybrid_termination_condition_ == HybridTerminationCondition::NO_IMPROVEMENT && component_improvement_found_ == false) {
        return TerminationCondition::MINIMUM_STEP_LENGTH_REACHED;
    }
    else if (case_handler_->CasesBeingEvaluated().size() > 0) {
        return TerminationCondition::NOT_FINISHED;
    }
    else if (iteration_ < max_hybrid_iterations_) {
        return TerminationCondition::NOT_FINISHED;
    }
    else {
        if (active_component_ == 0) {
            return TerminationCondition::NOT_FINISHED;
        }
        else {
            return secondary_->IsFinished();
        }
    }
}
void HybridOptimizer::handleEvaluatedCase(Case *c) {
    if (enable_logging_) {
        logger_->AddEntry(this);
    }
    if (active_component_ == 0) {
        primary_->handleEvaluatedCase(c);
        tentative_best_case_ = primary_->tentative_best_case_;
    }
    else {
        secondary_->handleEvaluatedCase(c);
        tentative_best_case_ = secondary_->tentative_best_case_;
    }

}
void HybridOptimizer::iterate() {
    if (active_component_ == 0) { // Primary is active.
        if (primary_->IsFinished() == TerminationCondition::NOT_FINISHED) { // Primary is not finished.
            if (VERB_OPT >= 1) { Printer::ext_info("Iterating with primary.", "HybridOptimizer", "Optimization"); }
            primary_->iterate();
        }
        else { // Primary is finished. Switch to secondary.
            if (VERB_OPT >= 1) { Printer::ext_info("Primary component converged. Switching to secondary.", "HybridOptimizer", "Optimization"); }
            primary_best_case_ = tentative_best_case_;
            initializeComponent(1);
            active_component_ = 1;
            secondary_->iterate();
            iteration_++;

            if (iteration_ > 1 && isBetter(primary_best_case_, secondary_best_case_) == false) {
                // If primary didn't find an improvement over previous secondary
                component_improvement_found_ = false;
            }
        }
    }
    else { // Secondary is active
        if (secondary_->IsFinished() == TerminationCondition::NOT_FINISHED) { // Secondary is not finished.
            if (VERB_OPT >= 1) { Printer::ext_info("Iterating with secondary.", "HybridOptimizer", "Optimization"); }
            secondary_->iterate();
        }
        else { // Secondary is finished. Switch back to primary.
            if (VERB_OPT >= 1) { Printer::ext_info("Secondary component converged. Switching to primary.", "HybridOptimizer", "Optimization"); }
            secondary_best_case_ = tentative_best_case_;
            initializeComponent(0);
            active_component_ = 0;
            primary_->iterate();

            if (isBetter(secondary_best_case_, primary_best_case_) == false) {
                // If secondary didn't find an improvement over previous primary
                component_improvement_found_ = false;
            }
        }
    }
}
void HybridOptimizer::initializeComponent(int component=0) {
    assert(component == 0 || component == 1);

    Settings::Optimizer *opt_settings;
    std::string compstr = "";
    if (component == 0) {
        opt_settings = primary_settings_;
        compstr = "primary";
    }
    else {
        opt_settings = secondary_settings_;
        compstr = "secondary";
    }

    Optimizer *opt;

    switch (opt_settings->type()) {
        case Settings::Optimizer::OptimizerType::Compass:
            std::cout << "Using Compass Search as " + compstr + " component in hybrid algorithm." << std::endl;
            opt = new Optimizers::CompassSearch(
                opt_settings, tentative_best_case_, variables_, grid_, logger_,
                case_handler_, constraint_handler_
            );
            break;
        case Settings::Optimizer::OptimizerType::APPS:
            std::cout << "Using APPS as " + compstr + " component in hybrid algorithm." << std::endl;
            opt = new Optimizers::APPS(
                opt_settings, tentative_best_case_, variables_, grid_, logger_,
                case_handler_, constraint_handler_
            );
            break;
        case Settings::Optimizer::OptimizerType::GeneticAlgorithm:
            std::cout << "Using Genetic Algorithm as " + compstr + " component in hybrid algorithm." << std::endl;
            opt = new Optimizers::RGARDD(
                opt_settings, tentative_best_case_, variables_, grid_, logger_,
                case_handler_, constraint_handler_
            );
            break;
        case Settings::Optimizer::OptimizerType::EGO:
            std::cout << "Using EGO as " + compstr + " component in hybrid algorithm." << std::endl;
            opt = new Optimizers::BayesianOptimization::EGO(
                opt_settings, tentative_best_case_, variables_, grid_, logger_,
                case_handler_, constraint_handler_
            );
            break;
        default:
            throw std::runtime_error("Unable to initialize hybrid optimizer: algorithm not recognized.");
    }
    opt->DisableLogging();
    if (component == 0)
        primary_ = opt;
    else
        secondary_ = opt;
}
}
