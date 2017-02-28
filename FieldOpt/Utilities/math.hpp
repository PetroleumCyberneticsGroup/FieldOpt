/// This file contains helper methods for working with maths.
#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

#include <QList>
#include <vector>
#include <boost/random.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/random.hpp>

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
 * @brief Get a random integer in the range [lower .. upper]
 * @param lower The lowest possible int.
 * @param upper The highest possible int.
 * @return A random integer.
 */
inline int random_integer(const int lower, const int upper) {
    boost::random::mt19937 gen;
    boost::random::uniform_int_distribution<> dist(lower, upper);
    return dist(gen);
}

/*!
 * @brief Generate a vector of n random floats in the range [0, 1)
 * @param n Number numbers to generate.
 * @return A vector containing n random floats.
 */
inline std::vector<float> random_floats(int n) {
    boost::random::uniform_01<boost::multiprecision::mpf_float_50> uf;
    boost::random::independent_bits_engine<boost::random::mt19937, 50L*1000L/301L, boost::multiprecision::mpz_int> gen;
    std::vector<float> rands = std::vector<float>(n);
    for (int i = 0; i < n; ++i) {
        rands[i] = static_cast<float>(uf(gen));
    }
    return rands;
}

#endif // MATH_FUNCTIONS_H
