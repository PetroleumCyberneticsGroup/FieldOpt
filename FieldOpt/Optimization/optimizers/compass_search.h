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

#ifndef COMPASSSEARCH_H
#define COMPASSSEARCH_H

#include "Optimization/optimizer.h"
#include "GSS.h"

namespace Optimization {
    namespace Optimizers {

/*!
 * \brief The CompassSearch class is an implementation
 * of the Compass Search optimization algorithm described
 * by Torczon, Lewis and Kolda. It is an extension of the
 * abstract GSS class.
 *
 * This algorithm only supports integer and real variables,
 * and not both at the same time.
 *
 * Reference:
 *
 * Kolda, Tamara G., Robert Michael Lewis, and Virginia Torczon.
 * "Optimization by direct search: New perspectives on some
 * classical and modern methods." SIAM review 45.3 (2003): 385-482.
 */
        class CompassSearch : public GSS
        {
        public:
            CompassSearch(::Settings::Optimizer *settings, Case *base_case,
                          ::Model::Properties::VariablePropertyContainer *variables,
                          Reservoir::Grid::Grid *grid);

            QString GetStatusStringHeader() const;
            QString GetStatusString() const;

        private:
          //!< Step or contract, perturb, and clear list
          //!< of recently evaluated cases.
          void iterate();

          //!< Check if this iteration was successful (i.e. if the
          //!< current tent. best case was found in this iteration).
          bool is_successful_iteration();

        protected:
            void handleEvaluatedCase(Case *c) override;
        };

    }}

#endif // COMPASSSEARCH_H
