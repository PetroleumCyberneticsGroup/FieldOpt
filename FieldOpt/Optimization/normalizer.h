/******************************************************************************
   Created by einar on 5/23/17.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#ifndef FIELDOPT_NORMALIZER_H
#define FIELDOPT_NORMALIZER_H

namespace Optimization {

/*!
 * @brief The Normalizer class facilitates normalization and de-normalization
 * of values. Currently, it only provides normalization using the Logistic
 * Function (https://en.wikipedia.org/wiki/Logistic_function):
 *
 * The Logistic Function is defined as
 * \f[
 *  f(x) = \frac{L}{1 + exp[-k(x - x_0)]}
 * \f]
 * Where
 *  - x    is the value to be normalized;
 *  - f(x) is the normalized value;
 *  - x_0  is the midpoint of the function;
 *  - L    is the maximum value of the curve;
 *  - k    is the steepness of the curve.
 *
 * Conversely, de-normalization is achieved by
 * \f[
 *  x(f(x)) = x_0 - \frac{log(L/f(x) - 1)}{k}
 * \f]
 */
class Normalizer {
 public:
  Normalizer();

  /*!
   * @brief Normalize a value using the Logistic function.
   * @param val The value to be normalized.
   * @return The normalized value.
   */
  long double normalize(const double val) const;

  /*!
   * @brief Calculate the de-normalized value from a normalized value using
   * the Logistic function solved for x.
   * @param nval The normalized value.
   * @return The de-normalized value.
   */
  double denormalize(const long double nval) const;

  /*!
   * @brief Check if the normalizer is ready, i.e. if all the parameters
   * have been set.
   * @return True if the normalizer is ready; otherwise false.
   */
  bool is_ready() const;

  //! Set the midpoint (x_0) parameter.
  void set_midpoint(const long double midpoint) { midpoint_ = midpoint; }

  //! Set the steepness (k) parameter.
  void set_steepness(const long double steepness) { steepness_  = steepness; }

  //! Set the max (L) parameter.
  void set_max(const long double max) { max_ = max; }

 private:
  long double midpoint_;  //!< The midpoint (x_0) for the logistic function.
  long double steepness_; //!< The steepness (k) for the logistic function.
  long double max_;       //!< The maximum value (L) of the logistic function.
};
}

#endif //FIELDOPT_NORMALIZER_H
