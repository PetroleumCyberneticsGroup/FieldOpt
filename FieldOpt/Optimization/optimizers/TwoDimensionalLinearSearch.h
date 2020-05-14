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

#include <boost/random.hpp>
#include "optimizer.h"

#ifndef FIELDOPT_TwoDimensionalLinearSearch_H
#define FIELDOPT_TwoDimensionalLinearSearch_H
namespace Optimization {
    namespace Optimizers {


/*!
 * @brief This class implements the particle swarm optimization (PSO) algorithm.
 *
 * The implementation is based on the description found at:
 * http://www.cleveralgorithms.com/nature-inspired/swarm/pso.html
 */
        class TwoDimensionalLinearSearch : public Optimizer {
        public:
            TwoDimensionalLinearSearch(Settings::Optimizer *settings,
                Case *base_case,
                Model::Properties::VariablePropertyContainer *variables,
                Reservoir::Grid::Grid *grid,
                Logger *logger,
                CaseHandler *case_handler=0,
                Constraints::ConstraintHandler *constraint_handler=0);
        protected:
            void handleEvaluatedCase(Case *c) override;
            void iterate() override;
            virtual TerminationCondition IsFinished() override;
        protected:
            boost::random::mt19937 gen_; //!< Random number generator with the random functions in math.hpp
        public:
            /*!
             * @brief
             * Generates a random set of cases within their given upper and lower bounds. The function also generates an initial
             * velocity based on the vMax parameter given through the .json file.
             * @return
             */
            Case *generateCase(double x, double y);

            int resolution_;
            double stagnation_limit_; //!< The stagnation criterion, standard deviation of all particle positions.
            Eigen::VectorXd v_max_; //!< Max velocity of the particle
            int max_iterations_; //!< Max iterations
            Eigen::VectorXd lower_bound_; //!< Lower bounds for the variables (used for randomly generating populations and mutation)
            Eigen::VectorXd upper_bound_; //!< Upper bounds for the variables (used for randomly generating populations and mutation)
            int n_vars_; //!< Number of variables in the problem.

        };
    }
}

#endif //FIELDOPT_PSO_H
