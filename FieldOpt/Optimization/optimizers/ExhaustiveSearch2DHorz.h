/******************************************************************************
   Copyright (C) 2015-2016 Mathias C. Bellout <mathias.bellout@ntnu.no>

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

#ifndef FIELDOPT_EXHAUSTIVESEARCH2DHORZ_H
#define FIELDOPT_EXHAUSTIVESEARCH2DHORZ_H

#include "Optimization/optimizer.h"

namespace Optimization {
namespace Optimizers {

/*!
 * @brief Performs approximate exhaustive search, i.e., it performs
 * a grid-type sampling that covers the entire solution space of
 * possible 2D positions of a singe horizontal well.
 *
 * Case objects for all combinations are created and launched in
 * parallel.
 */

class ExhaustiveSearch2DHorz {

};

}
}

#endif //FIELDOPT_EXHAUSTIVESEARCH2DHORZ_H
