/******************************************************************************
   Created by einar on 11/15/16.
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
#ifndef FIELDOPT_TEST_RESOURCE_TEST_FUNCTIONS_H
#define FIELDOPT_TEST_RESOURCE_TEST_FUNCTIONS_H

#include <Eigen/Core>

using namespace Eigen;

namespace TestResources {
    /*!
     * @brief This namespace contains synthetic functions meant to be used
     * for testQing optimzation algorithms.
     *
     * Note that these are all formulated for minimization.
     */
    namespace TestFunctions {

        /*!
         * @brief Sphere function.
         *
         * Formula: \f$ \sum_{i=1}^{n} x^2_i \f$
         * Minimum: \f$ f(x_1, ..., x_n) = f(0, ..., 0) = 0 \f$
         * Domain:  \f$ - \infty \leq x_i \leq \infty \f$
         * Dimensions: \f$ 1 \leq i \leq n \f$
         *
         * @param xs Vector of _continous_ variable values.
         * @return The function value at the given positon.
         */
        inline double Sphere(VectorXd xs) {
            return (xs.cwiseProduct(xs)).sum();
        }

        /*!
         * @brief Rosenbrock function.
         *
         * Formula: \f$ \sum_{i=1}^{n-1} \left[ 100 (x_{i+1} - x_i^2)^2 + (x_i -1)^2 \right] \f$
         * Minimum: \f$ f(1, ..., 1) = 0 \f$
         * Domain:  \f$ - \infty \leq x_i \leq \infty \f$
         * Dimensions: \f$ 1 \leq i \leq n \f$
         *
         * @param xs Vector of _continous_ variable values.
         * @return The function value at the given position.
         */
        inline double Rosenbrock(VectorXd xs) {
            VectorXd xhead = xs.head(xs.size() - 1);
            VectorXd xtail = xs.tail(xs.size() - 1);

            VectorXd p1 = xtail - xhead.cwiseProduct(xhead);
            VectorXd p2 = xhead - VectorXd::Ones(xhead.size());
            return (100 * p1.cwiseProduct(p1) + p2.cwiseProduct(p2)).sum();

        }

    }
}

#endif //FIELDOPT_TEST_RESOURCE_TEST_FUNCTIONS_H
