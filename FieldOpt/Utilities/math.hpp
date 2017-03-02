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

#include <QList>
#include <vector>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <time.h>

/*!
 * @brief Calculate the average value of the items in the list. The returned value will always be a double.
 * @param list
 * @return The average value of the elements in the list as a double.
 */
template<typename T>
inline double calc_average(const QList<T> list) {
    assert(!list.empty());
    return std::accumulate(list.begin(), list.end(), 0.0) / list.size();
}

/*!
 * @brief Calculate the median of a list. The returned value will have the same type as the elements in the list.
 * If the values are integers and the list has an even number of elements, the result will be floored.
 * @param list
 * @return
 */
template<typename T>
inline T calc_median(QList<T> list) {
    assert(!list.empty());
    std::sort(list.begin(), list.end());
    size_t size = list.size();
    if (size % 2 == 0)
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
    int length = abs((end - start) / step);
    auto ran = std::vector<T>(length);
    ran[0] = start;
    for (int i = 1; i < length; ++i) {
        ran[i] = ran[i-1] + step;
    }
    return ran;
}

/*!
 * @brief Get a random generator (Mersenne Twister) for use with the random functions in this file.
 * @return A Mersenne Twister RNG.
 */
inline boost::random::mt19937 get_random_generator() {
    boost::random_device dev;
    boost::random::mt19937 gen(dev);
    return gen;
}

/*!
 * @brief Get a random integer in the range [lower .. upper]
 * @param gen Random number generator. You can get one by calling get_random_generator().
 * @param lower The lowest possible int.
 * @param upper The highest possible int.
 * @return A random integer.
 */
inline int random_integer(boost::random::mt19937 &gen, const int lower, const int upper) {
    boost::random::uniform_int_distribution<> dist(lower, upper);
    boost::variate_generator<boost::mt19937&, boost::random::uniform_int_distribution<> > rng(gen, dist);
    return rng();
}

/*!
 * @brief Generate a vector of random integers in the range [lower .. upper].
 * @param gen Random number generator. You can get one by calling get_random_generator().
 * @param lower The lowest possible int to generate.
 * @param upper The highest possible int to generate.
 * @param n Number of integers to generate.
 * @return A vector containing n random integers.
 */
inline std::vector<int> random_integers(boost::random::mt19937 &gen, const int lower, const int upper, const int n) {
    boost::random::uniform_int_distribution<> dist(lower, upper);
    boost::variate_generator<boost::mt19937&, boost::random::uniform_int_distribution<> > rng(gen, dist);

    std::vector<int> rands = std::vector<int>(n);
    for (int i = 0; i < n; ++i) {
        rands[i] = rng();
    }
    return rands;
}

/*!
 * @brief Generate a vector of n random floats in the range [0, 1)
 * @param gen Random number generator. You can get one by calling get_random_generator().
 * @param n Number numbers to generate.
 * @return A vector containing n random floats.
 */
inline std::vector<float> random_floats(boost::random::mt19937 &gen, const int n) {
    boost::uniform_real<> dist(0.0, 1.0);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rng(gen, dist);

    std::vector<float> rands = std::vector<float>(n);
    for (int i = 0; i < n; ++i) {
        rands[i] = rng();
    }
    return rands;
}

/*!
 * @brief Generate a vector of n random floats in the range [min, max)
 * @param gen Random number generator. You can get one by calling get_random_generator().
 * @param min Min limit for generated numbers.
 * @param max Max limit for generated numbers.
 * @param n Number numbers to generate.
 * @return A vector containing n random floats.
 */
inline std::vector<double> random_doubles(boost::random::mt19937 &gen, const double min, const double max, const int n) {
    boost::uniform_real<> dist(min, max);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rng(gen, dist);

    std::vector<double> rands = std::vector<double>(n);
    for (int i = 0; i < n; ++i) {
        rands[i] = rng();
    }
    return rands;
}

#endif // MATH_FUNCTIONS_H
