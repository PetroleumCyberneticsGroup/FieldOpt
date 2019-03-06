/******************************************************************************
   Created by einar on 2/27/17.
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

/// This file contains helper methods for working with maths.
#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

#include <vector>
#include <time.h>
#include <Eigen/Core>
#include <numeric>

/*!
 * @brief Calculate the average value of the items in the list. The returned value will always be a double.
 * @param list
 * @return The average value of the elements in the list as a double.
 */

template<typename T>
inline double calc_sum(std::vector<T> sums){
    return std::accumulate(sums.begin(), sums.end(), 0.0);
}
template<typename T>
inline double calc_average(const std::vector<T> list) {
    assert(!list.empty());
    return calc_sum(list) / list.size();
}

/*!
 * @brief Calculate the median of a list. The returned value will have the same type as the elements in the list.
 * If the values are integers and the list has an even number of elements, the result will be floored.
 * @param list
 * @return
 */
template<typename T>
inline T calc_median(std::vector<T> list) {
    assert(!list.empty());
    std::sort(list.begin(), list.end());
    size_t size = list.size();
    if (size == 1)
        return list[0];
    else if (size % 2 == 0)
        return (list[size/2 - 1] + list[size/2])/2;
    else
        return list[size/2];
}

/*!
 * @brief Linspace function. Create a list containing all elements in the range from start to (but not including) end
 * with step between each element.
 *
 * Examples: range(1, 10, 1) = [1, 2, 3, 4, 5, 6, 7, 8, 9]
 *           range(0, 10, 3) = [0, 3, 6, 9]
 * @tparam T A number type, e.g. int or double.
 * @param start First number in range.
 * @param end End of range.
 * @param step Step length.
 * @return
 */
template <typename T>
inline std::vector<T> range(T start, T end, T step) {
    int length = std::abs((end - start) / step);
    auto ran = std::vector<T>(length);
    ran[0] = start;
    for (int i = 1; i < length; ++i) {
        ran[i] = ran[i-1] + step;
    }
    return ran;
}
inline double calc_variance(std::vector<double> sums){
    std::vector<double> diff(sums.size());
    transform(sums.begin(), sums.end(), diff.begin(),
              bind2nd(std::minus<double>(), calc_average(sums))
    );
    double sq_sum = inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    return sq_sum/(sums.size()-1);

}

inline double calc_standard_deviation(std::vector<double> sums){
    double sq_sum = calc_variance(sums);
    double stdev = sqrt(sq_sum);
    return stdev;
}
#endif // MATH_FUNCTIONS_H
