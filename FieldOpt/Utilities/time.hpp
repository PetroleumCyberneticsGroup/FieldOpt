/// This file contains helper methods for working with time.
#ifndef TIME_FUNCTIONS
#define TIME_FUNCTION

/*!
 * @brief Get the number of seconds from t1 to t2.
 * @param t1 Fist time. Should be before t2.
 * @param t2 Seconds time. Should be after t1.
 */
int time_span_seconds(QDateTime t1, QDateTime t2) {
    return t2.toMSecsSinceEpoch()/1000 - t1.toMSecsSinceEpoch()/1000;
}

/*!
 * @brief Get the number of seconds that have passed from t1 until now.
 * @param t The time to get the number of seconds from.
 */
int time_since_seconds(QDateTime t) {
    return time_span_seconds(t, QDateTime::currentDateTime());
}

#endif // TIME_FUNCTIONS
