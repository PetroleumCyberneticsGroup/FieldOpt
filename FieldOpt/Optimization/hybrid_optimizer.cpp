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

#include <optimizers/compass_search.h>
#include <optimizers/APPS.h>
#include <optimizers/RGARDD.h>
#include <optimizers/bayesian_optimization/EGO.h>
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

    // TODO: Instantiate primary (needs a new constructor that takes existing Case- and ConstraintHandler objects)
    initializeComponent(0);
    active_component_ = 0;
    iteration_ = active_component_;
}

Optimizer::TerminationCondition HybridOptimizer::IsFinished() {
    if (primary_->IsFinished()) {
        if (active_component_ == 1 && secondary_->IsFinished()) {
            return secondary_->IsFinished();
        }
        else {
            return TerminationCondition::NOT_FINISHED;
        }
    }
    else {
        return TerminationCondition::NOT_FINISHED;
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
    if (active_component_ == 0 && !primary_->IsFinished()) {
        primary_->iterate();
    }
        // TODO: Initailize secondary if necessary.
    else {
        if (active_component_ == 0) {
            std::cout << "Primary component converged. Switching to secondary." << std::endl;
            initializeComponent(1);
            active_component_ = 1;
            iteration_ = active_component_;
        }
        if (case_handler_->QueuedCases().size() == 0) {
            secondary_->iterate();
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
