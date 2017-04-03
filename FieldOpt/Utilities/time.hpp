/// This file contains helper methods for working with time.
#ifndef TIME_FUNCTIONS_H
#define TIME_FUNCTIONS_H

#include <QDateTime>

/*!
 * @brief Get the number of seconds from t1 to t2.
 * @param t1 Fist time. Should be before t2.
 * @param t2 Seconds time. Should be after t1.
 */
inline int time_span_seconds(const QDateTime t1, const QDateTime t2) {
    return t2.toMSecsSinceEpoch()/1000 - t1.toMSecsSinceEpoch()/1000;
}

/*!
 * @brief Get the number of seconds that have passed from t1 until now.
 * @param t The time to get the number of seconds from.
 */
inline int time_since_seconds(const QDateTime t) {
    return time_span_seconds(t, QDateTime::currentDateTime());
}

/*!
 * @brief Get a time stamp string formatted as YYYY-MM-ddTHH:mm:ss
 * @return
 */
inline std::string timestamp_string() {
    return QDateTime::currentDateTime().toString(Qt::ISODate).toStdString();
}

/*!
 * @brief Convert seconds to a time span string formatted as HH:mm:ss
 * @param seconds Time span in seconds to convert.
 * @return Time span string.
 */
inline std::string timespan_string(int seconds) {
    int hrs = seconds/3600;
    int mins = (seconds - hrs*3600) / 60;
    int secs = seconds % 60;
    std::string tstamp = std::to_string(hrs) + ":" + std::to_string(mins) + ":" + std::to_string(secs);
    return tstamp;
}

#endif // TIME_FUNCTIONS_H
