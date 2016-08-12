/// This file contains helper methods for working with maths.
#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

#include <QList>

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

#endif // MATH_FUNCTIONS_H
