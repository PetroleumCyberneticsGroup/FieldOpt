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
#ifndef FIELDOPT_GSS_PATTERNS_HPP_H
#define FIELDOPT_GSS_PATTERNS_HPP_H

#include <Eigen/Core>
#include <vector>

namespace Optimization { namespace GSSPatterns {

/*!
 * @brief Get the set of search directions containing all coordinate
 * directions, bot positive and negative.
 *
 * This is the set used by CompassSearch.
 *
 * For 2D, the set is:
 * [1, 0], [0, 1], [-1, 0], [0, -1]
 *
 * Which looks like +.
 *
 * @param num_vars The number of variables in the problem.
 * @return The set of search directions in all coordinate directions,
 * positive and negative.
 */
inline std::vector<Eigen::VectorXi> Compass(int num_vars) {
    auto directions = std::vector<Eigen::VectorXi>(2*num_vars);
    for (int i = 0; i < num_vars; ++i) {
        Eigen::VectorXi dir = Eigen::VectorXi::Zero(num_vars);
        dir(i) = 1;
        directions[i] = dir;
        directions[i+num_vars] = (-1) * dir;
    }
    return directions;
}

}
}

#endif //FIELDOPT_GSS_PATTERNS_HPP_H
