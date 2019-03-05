/******************************************************************************
   Created by einar on 2/27/19.
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>

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

/// This file contains helper methods for working with random numbers.
#ifndef FIELDOPT_RANDOM_H
#define FIELDOPT_RANDOM_H

#include <vector>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <Eigen/Core>

/*!
 * @brief Get a random generator (Mersenne Twister) for use with the random functions in this file.
 * @return A Mersenne Twister RNG.
 */
inline boost::random::mt19937 get_random_generator(int seed = 0) {
    if (seed == 0) {
        boost::random_device dev;
        boost::random::mt19937 gen(dev);
        return gen;
    }
    else {
        boost::random::mt19937 gen(seed);
        return gen;
    }
}


/*!
 * @brief Get a random integer in the range [lower .. upper]
 * @param gen Random number generator. You can get one by calling get_random_generator().
 * @param lower The lowest possible int.
 * @param upper The highest possible int.
 * @return A random integer.
 */
inline int random_integer(boost::random::mt19937 &gen,
                          const int lower, const int upper) {
    boost::random::uniform_int_distribution<> dist(lower, upper);
    boost::variate_generator<boost::mt19937&, boost::random::uniform_int_distribution<> > rng(gen, dist);
    return rng();
}


inline std::vector<double> random_reals(boost::random::mt19937 &gen, double lower, double upper, int n) {
    boost::random::uniform_real_distribution<> dist(lower, upper);
    boost::variate_generator<boost::mt19937&, boost::random::uniform_real_distribution<> > rng(gen, dist);
    std::vector<double> rands = std::vector<double>(n);
    for (int i = 0; i < n; ++i){
        rands[i]=rng();
    }
    return rands;
}

/*!
 * @brief Generate a vector of random integers in the range [lower .. upper].
 * @param gen Random number generator. You can get one by calling get_random_generator().
 * @param lower The lowest possible int to generate.
 * @param upper The highest possible int to generate.
 * @param n Number of integers to generate.
 * @return A vector containing n random integers.
 */
inline std::vector<int> random_integers(boost::random::mt19937 &gen,
                                        const int lower, const int upper,
                                        const int n) {
    boost::random::uniform_int_distribution<> dist(lower, upper);
    boost::variate_generator<boost::mt19937&, boost::random::uniform_int_distribution<> > rng(gen, dist);

    std::vector<int> rands = std::vector<int>(n);
    for (int i = 0; i < n; ++i) {
        rands[i] = rng();
    }
    return rands;
}

/*!
 * @brief Generate a vector of _unique_ random integers in the range [lower .. upper].
 * @param gen Random number generator. You can get one by calling get_random_generator().
 * @param lower The lowest possible int to generate.
 * @param upper The highest possible int to generate.
 * @param n Number of integers to generate.
 * @return A vector containing n random integers.
 */
inline std::vector<int> unique_random_integers(boost::random::mt19937 &gen,
                                               const int lower, const int upper,
                                               const int n) {
    boost::random::uniform_int_distribution<> dist(lower, upper);
    boost::variate_generator<boost::mt19937&, boost::random::uniform_int_distribution<> > rng(gen, dist);

    std::vector<int> rands = std::vector<int>(n);
    int i = 0;
    while (i < n) {
        int rand = rng();
        if(!rands.empty() && std::find(rands.begin(), rands.end(), rand) != rands.end()) {
            continue;
        }
        else {
            rands[i] = rand;
            ++i;
        }
    }
    return rands;
}

/*!
 * @brief Generate a vector of n random floats in the range [0, 1)
 * @param gen Random number generator. You can get one by calling get_random_generator().
 * @param n Number numbers to generate.
 * @return A vector containing n random floats.
 */
inline std::vector<float> random_floats(boost::random::mt19937 &gen,
                                        const int n) {
    boost::uniform_real<> dist(0.0, 1.0);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rng(gen, dist);

    std::vector<float> rands = std::vector<float>(n);
    for (int i = 0; i < n; ++i) {
        rands[i] = rng();
    }
    return rands;
}

/*!
 * @brief Generate an n-element vector where each element is +1 or -1 with equivalent
 * probability (i.e. a symmetric Bernoulli distribution).
 * @param gen Random number generator.
 * @param n Number of elements to generate.
 */
inline std::vector<int> random_symmetric_bernoulli(boost::random::mt19937 &gen, const int n) {
    std::vector<int> numbers(n);
    boost::bernoulli_distribution<> dist;
    boost::variate_generator<boost::mt19937&, boost::bernoulli_distribution<> > rng(gen, dist);
    for (int i = 0; i < n; i++) {
        auto rn = rng();
        numbers[i] = 2 * rn - 1;
    }
    return numbers;
}

/*!
 * @brief Calls random_symmetric_bernoulli to generate a vector and converts it to an Eigen vector.
 * @param gen Random number generator.
 * @param n Number of elements to generate.
 */
inline Eigen::VectorXd random_symmetric_bernoulli_eigen(boost::random::mt19937 &gen, const int n) {
    auto numbers = random_symmetric_bernoulli(gen, n);
    auto numbers_eigen = Eigen::VectorXd(n);
    for (int i = 0; i < n; i++) {
        numbers_eigen[i] = numbers[i];
    }
    return numbers_eigen;
}

/*!
 * @brief Generate a vector of n random floats in the range [min, max)
 * @param gen Random number generator. You can get one by calling get_random_generator().
 * @param min Min limit for generated numbers.
 * @param max Max limit for generated numbers.
 * @param n Number numbers to generate.
 * @return A vector containing n random floats.
 */
inline std::vector<double> random_doubles(boost::random::mt19937 &gen,
                                          const double min, const double max,
                                          const int n) {
    boost::uniform_real<> dist(min, max);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rng(gen, dist);

    std::vector<double> rands = std::vector<double>(n);
    for (int i = 0; i < n; ++i) {
        rands[i] = rng();
    }
    return rands;
}

/*!
 * @brief Generate a random double in the range [min, max)
 * @param gen Random number generator. You can get one by calling get_random_generator().
 * @param min Min limit for generated number.
 * @param max Max limit for generated number.
 * @return A random double.
 */
inline double random_double(boost::random::mt19937 &gen, const double min, const double max) {
    boost::uniform_real<> dist(min, max);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rng(gen, dist);
    double rand = rng();
    return rand;
}

/*!
 * @brief Generate an Eigen vector of n random floats in the range [min, max)
 * @param gen Random number generator. You can get one by calling get_random_generator().
 * @param min Min limit for generated numbers.
 * @param max Max limit for generated numbers.
 * @param n Number numbers to generate.
 * @return A vector containing n random floats.
 */
inline Eigen::VectorXd random_doubles_eigen(boost::random::mt19937 &gen,
                                            const double min, const double max,
                                            const int n) {
    boost::uniform_real<> dist(min, max);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rng(gen, dist);

    Eigen::VectorXd rands(n);
    for (int i = 0; i < n; ++i) {
        rands(i) = rng();
    }
    return rands;
}

/*!
 * @brief Generate a random float (double precision) in the range [0.0 .. 1.0)
 * @param gen
 * @return One random float.
 */
inline double random_double(boost::random::mt19937 &gen) {
    boost::uniform_real<> dist(0.0, 1.0);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rng(gen, dist);
    return rng();
}


#endif //FIELDOPT_RANDOM_H
