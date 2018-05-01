/// This file contains helper methods for working with time.
#ifndef TIME_FUNCTIONS_H
#define TIME_FUNCTIONS_H

#include <QDateTime>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include "Utilities/colors.hpp"

// -----------------------------------------------------------------
/*!
 * @brief Get the number of seconds from t1 to t2.
 * @param t1 Fist time. Should be before t2.
 * @param t2 Seconds time. Should be after t1.
 */
inline int time_span_secs(const QDateTime t1,
                          const QDateTime t2) {
  double start = t1.toMSecsSinceEpoch() / 1000.0;
  double end = t2.toMSecsSinceEpoch() / 1000.0;
  return end - start;
}

//// -----------------------------------------------------------------
//inline const clock_t set_tstart() {
//  clock_t tstart = std::clock();
//  return tstart;
//}
//
//// -----------------------------------------------------------------
//inline double get_tend(clock_t tstart) {
//  double tend = float(std::clock() - tstart) /  CLOCKS_PER_SEC;
//  std::cout << "TIME: " << tend << std::endl;
//  return tend;
//}

// -----------------------------------------------------------------
/*!
 * @brief Get the number of milliseconds from t1 to t2.
 * @param t1 Fist time. Should be before t2.
 * @param t2 Seconds time. Should be after t1.
 */
inline uint time_span_msecs(const QDateTime t1,
                            const QDateTime t2) {
  qint64 start = t1.toMSecsSinceEpoch();
  qint64 end = t2.toMSecsSinceEpoch();
  auto tdiff = end - start;
  return tdiff;

}

// -----------------------------------------------------------------
/*!
 * @brief Get the number of seconds that have passed from
 * t1 until now.
 * @param t The time to get the number of seconds from.
 */
inline int time_since_secs(const QDateTime t) {
  return time_span_secs(t, QDateTime::currentDateTime());
}

// -----------------------------------------------------------------
/*!
 * @brief Get the number of milliseconds that have passed
 * from t1 until now.
 * @param t The time to get the number of seconds from.
 */
inline int time_since_msecs(const QDateTime t) {
  return time_span_msecs(t, QDateTime::currentDateTime());
}

// -----------------------------------------------------------------
/*!
 * @brief Get a time stamp string
 * formatted as YYYY-MM-ddTHH:mm:ss
 * @return
 */
inline std::string timestamp_string() {
  return QDateTime::currentDateTime().toString(Qt::ISODate).toStdString();
}

// -----------------------------------------------------------------
inline std::string timestamp_string(const QDateTime dt) {
  return dt.toString(Qt::ISODate).toStdString();
}

// -----------------------------------------------------------------
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

/*!
 * @brief Convert UNIX time (seconds since Thursday, 1 January 1970)
 * to a date string in the format YYYY.MM.DD.
 * @param t UNIX Time
 * @return Date string
 */
inline std::string unix_time_to_datestring(const time_t &t) {
    std::tm * ptm = std::localtime(&t);
    char buffer[11];
    std::strftime(buffer, 11, "%Y.%m.%d", ptm);
    return std::string(buffer);
}

#endif // TIME_FUNCTIONS_H
