/// This file contains helper methods for working with time.
#ifndef TIME_FUNCTIONS_H
#define TIME_FUNCTIONS_H

/*!
 * @brief Get the number of seconds from t1 to t2.
 * @param t1 Fist time. Should be before t2.
 * @param t2 Seconds time. Should be after t1.
 */
inline int time_span_seconds(const QDateTime t1, const QDateTime t2) {
    return t2.toMSecsSinceEpoch()/1000 - t1.toMSecsSinceEpoch()/1000;
}

/*!
 * @brief Get the number of milliseconds from t1 to t2.
 * @param t1 Fist time. Should be before t2.
 * @param t2 Seconds time. Should be after t1.
 */
inline int time_span_milliseconds(const QDateTime t1, const QDateTime t2) {
    return t2.toMSecsSinceEpoch() - t1.toMSecsSinceEpoch();
}

/*!
 * @brief Get the number of seconds that have passed from t1 until now.
 * @param t The time to get the number of seconds from.
 */
inline int time_since_seconds(const QDateTime t) {
    return time_span_seconds(t, QDateTime::currentDateTime());
}

/*!
 * @brief Get the number of milliseconds that have passed from t1 until now.
 * @param t The time to get the number of seconds from.
 */
inline int time_since_milliseconds(const QDateTime t) {
    return time_span_milliseconds(t, QDateTime::currentDateTime());
}

#endif // TIME_FUNCTIONS_H
