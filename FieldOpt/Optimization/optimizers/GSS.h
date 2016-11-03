/******************************************************************************
   Created by einar on 11/3/16.
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
#ifndef FIELDOPT_GSS_H
#define FIELDOPT_GSS_H

#include "Optimization/optimizer.h"
#include <Eigen/Core>
#include <vector>

using namespace Eigen;
using namespace std;

namespace Optimization {
    namespace Optimizers {

        /*!
         * @brief The _abstract_ GSS class implements a generalized form of the
         * Generating Set Search algorithm presented by Kolda, Torczon and Lewis
         * in the 2003 paper Optimization By Direct Search: New perspectives on
         * some classical and modern methods (DOI: 10.1137/S003614450242889)
         *
         * This implementation also borrows some from the Kolda's 2005 paper on
         * the Asynchronous Parallel Pattern Search (APPS) (DOI: 0.1137/040603589)
         * in that it allows for multiple current step lengths (e.g. one per search
         * direction). This is to allow for more flexibility in the implementation
         * of specific algorithms (e.g. APPS).
         *
         * @note that this is an abstract class. It must be extended by some other
         * class (e.g. like the CompassSearch class) to provide specific patterns
         * and contraction/expansion parameter values.
         */
        class GSS : public Optimizer {
        public:
            GSS(Settings::Optimizer *settings, Case *base_case, Model::Properties::VariablePropertyContainer *variables,
                Reservoir::Grid::Grid *grid);

        private:
            double step_tol_; //!< Step length convergence tolerance.
            double contr_fac_; //!< Step length contraction factor.
            double expan_fac_; //!< Step length expansion factor.
            VectorXd step_length_; //!< Vector of step lengths.
            vector<VectorXd> directions_; //!< Vector of search directions.
        };

    }
}
#endif //FIELDOPT_GSS_H
