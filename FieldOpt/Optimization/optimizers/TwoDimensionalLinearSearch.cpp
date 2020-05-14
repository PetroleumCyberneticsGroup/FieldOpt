/******************************************************************************
   Created by Brage on 08/11/18.
   Copyright (C) 2018 Brage Strand Kristoffersen <brage_sk@hotmail.com>

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

#include "TwoDimensionalLinearSearch.h"
#include "Utilities/math.hpp"
#include "Utilities/random.hpp"
#include "Utilities/stringhelpers.hpp"
#include <math.h>

namespace Optimization{
    namespace Optimizers{
        TwoDimensionalLinearSearch::TwoDimensionalLinearSearch(Settings::Optimizer *settings,
                 Case *base_case,
                 Model::Properties::VariablePropertyContainer *variables,
                 Reservoir::Grid::Grid *grid,
                 Logger *logger,
                 CaseHandler *case_handler,
                 Constraints::ConstraintHandler *constraint_handler
        ) : Optimizer(settings, base_case, variables, grid, logger, case_handler, constraint_handler) {
            n_vars_ = variables->ContinousVariableSize();
            gen_ = get_random_generator(settings->parameters().rng_seed);
            max_iterations_ = 1;
            resolution_ = settings->parameters().TDLS_resolution_;
            if (constraint_handler_->HasBoundaryConstraints()) {
                lower_bound_ = constraint_handler_->GetLowerBounds(base_case->GetRealVarIdVector());
                upper_bound_ = constraint_handler_->GetUpperBounds(base_case->GetRealVarIdVector());
            }
            else {
                lower_bound_.resize(n_vars_);
                upper_bound_.resize(n_vars_);
                lower_bound_.fill(settings->parameters().lower_bound);
                upper_bound_.fill(settings->parameters().upper_bound);
            }
            auto difference = upper_bound_ - lower_bound_;

            if (n_vars_ == 2){
                std::vector<double> x_axis = std::vector<double>(resolution_);
                std::vector<double> y_axis = std::vector<double>(resolution_);

                for (int i = 0; i < resolution_; ++i) {
                    x_axis[i] = (difference[0]/resolution_)*i+lower_bound_[0];
                    y_axis[i] = (difference[1]/resolution_)*i+lower_bound_[1];
                }
                for (int i = 0; i < x_axis.size(); i++){
                    for (int j = 0; j < y_axis.size(); j++){
                        auto new_case = generateCase(x_axis[i], y_axis[j]);
                        case_handler_->AddNewCase(new_case);
                    }
                }
            }

        }

        void TwoDimensionalLinearSearch::iterate(){
            if(enable_logging_){
                logger_->AddEntry(this);
            }

            iteration_++;
        }

        void TwoDimensionalLinearSearch::handleEvaluatedCase(Case *c) {
            if(isImprovement(c)){
                updateTentativeBestCase(c);
                if (VERB_OPT > 1) {
                    stringstream ss;
                    ss.precision(6);
                    ss << scientific;
                    ss << "New best in swarm, iteration " << Printer::num2str(iteration_) << ": OFV " << c->objective_function_value();
                    Printer::ext_info(ss.str(), "Optimization", "PSO");
                }
            }
        }

        Optimizer::TerminationCondition TwoDimensionalLinearSearch::IsFinished() {
            if (case_handler_->CasesBeingEvaluated().size() > 0) return NOT_FINISHED;
            if (iteration_ < max_iterations_) return NOT_FINISHED;
            else return MAX_EVALS_REACHED;
        }

        Case *TwoDimensionalLinearSearch::generateCase(double x, double y) {
            Case *new_case;
            new_case = new Case(GetTentativeBestCase());
            Eigen::VectorXd erands(n_vars_);
            erands(0) = x;
            erands(1) = y;
            new_case->SetRealVarValues(erands);
            return  new_case;
        }
    }
}
