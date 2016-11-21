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

            set<int> active_; //!< Set containing the indices of all active search directions.

            vector<int> inactive(); //!< Get vector containing all _inactive_ search directions.
        };

    }
}


#endif //FIELDOPT_APPS_H
