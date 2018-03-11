#include <gtest/gtest.h>
#include <QtCore/QDate>
#include "Utilities/time.hpp"

namespace {

    class TimeTest : public testing::Test {

    };


    TEST_F(TimeTest, TimeSpan) {
        int time_10_seconds;
        QDate date = QDate(2016, 07, 26);
        QTime time1 = QTime(10, 10, 0);
        QTime time2 = QTime(10, 10, 10);
        auto t1 = QDateTime(date, time1, Qt::UTC, 0);
        auto t2 = QDateTime(date, time2, Qt::UTC, 0);
        EXPECT_EQ(10, time_span_secs(t1, t2));
    }
}
