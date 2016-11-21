/******************************************************************************
   Created by einar on 11/21/16.
   Copyright (C) 2016 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#ifndef FIELDOPT_APPS_H
#define FIELDOPT_APPS_H

#include <set>
#include "GSS.h"

namespace Optimization {
    namespace Optimizers {

        class APPS : public GSS {
        public:
            APPS(Settings::Optimizer *settings, Case *base_case,
                 Model::Properties::VariablePropertyContainer *variables, Reservoir::Grid::Grid *grid);

        protected:
            void handleEvaluatedCase(Case *c) override;

            void iterate() override;

        private:
            int max_queue_length_; //!< Maximum length of queue.
            set<int> active_; //!< Set containing the indices of all active search directions.
            void set_active(vector<int> dirs); //!< Mark the direction indices in the vector as active.
            void set_inactive(vector<int> dirs); //!< Mark the direction indices in the vector as inactive.
            void reset_active(); //!< Reset the list of active search directions.
            vector<int> inactive(); //!< Get vector containing all _inactive_ search directions.

            /*!
             * @brief Handle a successful iteration.
             *
             * Will be called by handleEvaluatedCase() if the most recently evaluated case is an improvement
             * on the tentative_best_case_.
             * @param c Most recently evaluated case.
             */
            void successful_iteration(Case *c);

            /*!
             * @brief Handle an unsuccessful iteration.
             *
             * Will be called by handleEvaluatedCase() if the most recently evaluated case is _not_ and improvement
             * ono the tentative_best_case_.
             * @param c Most recently evaluated case.
             */
            void unsuccessful_iteration(Case *c);

            /*!
             * @brief Prune the evaluation queue to max_queue_length_.
             */
            void prune_queue();
        };

    }
}


#endif //FIELDOPT_APPS_H
