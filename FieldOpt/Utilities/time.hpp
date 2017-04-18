/// This file contains helper methods for working with time.
#ifndef TIME_FUNCTIONS_H
#define TIME_FUNCTIONS_H

#include <QDateTime>
#include <sstream>
#include <iomanip>

/*!
 * @brief Get the number of seconds from t1 to t2.
 * @param t1 Fist time. Should be before t2.
 * @param t2 Seconds time. Should be after t1.
 */
inline int time_span_seconds(const QDateTime t1, const QDateTime t2) {
    double start = t1.toMSecsSinceEpoch() / 1000.0;
    double end = t1.toMSecsSinceEpoch() / 1000.0;
    return end - start;
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

inline std::string timestamp_string(const QDateTime dt) {
    return dt.toString(Qt::ISODate).toStdString();
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

    std::stringstream timestr;
    timestr << std::setfill('0') << std::setw(2) << hrs << ":";
    timestr << std::setfill('0') << std::setw(2) << mins << ":";
    timestr << std::setfill('0') << std::setw(2) << secs;

    return timestr.str();
}

#endif // TIME_FUNCTIONS_H
