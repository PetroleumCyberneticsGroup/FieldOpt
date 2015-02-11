/*
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einarjba@stud.ntnu.no>
 *
 * The code is largely based on ResOpt, created by  Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "teststream.h"



void TestStream::initTestCase()
{
    stream1 = new Stream;
    stream2 = new Stream(0.0, 10.0, 20.0, 30.0, 100.0);
}

void TestStream::testConstructor()
{
    QVERIFY(stream2->time() == 0.0);
    QVERIFY(stream2->oilRate() == 10.0);
    QVERIFY(stream2->gasRate() == 20.0);
    QVERIFY(stream2->waterRate() == 30.0);
    QVERIFY(stream2->pressure() == 100.0);
    QVERIFY(stream2->inputUnits() == Stream::FIELD);
}

void TestStream::testSetters()
{
    stream1->setTime(5.0);
    stream1->setOilRate(15.0);
    stream1->setGasRate(25.0);
    stream1->setWaterRate(35.0);
    stream1->setPressure(105.0);
    stream1->setInputUnits(Stream::METRIC);

    QVERIFY(stream1->time() == 5.0);
    QVERIFY(stream1->oilRate() == 15.0);
    QVERIFY(stream1->gasRate() == 25.0);
    QVERIFY(stream1->waterRate() == 35.0);
    QVERIFY(stream1->pressure() == 105.0);
    QVERIFY(stream1->inputUnits() == Stream::METRIC);
}

void TestStream::testAssignmentOperator()
{
    Stream* stream3 = stream1;
    QVERIFY(stream3->time() == 5.0);
    QVERIFY(stream3->oilRate() == 15.0);
    QVERIFY(stream3->gasRate() == 25.0);
    QVERIFY(stream3->waterRate() == 35.0);
    QVERIFY(stream3->pressure() == 105.0);
    QVERIFY(stream3->inputUnits() == Stream::METRIC);
}

void TestStream::testCompoundAssignmentOperator()
{
    double newOilRate = stream1->oilRate(stream2->inputUnits()) + stream2->oilRate();
    double newGasRate = stream1->gasRate(stream2->inputUnits()) + stream2->gasRate();
    double newWaterRate = stream1->waterRate(stream2->inputUnits()) + stream2->waterRate();
    double newPressure = stream1->pressure(stream2->inputUnits()) + stream2->pressure();

    *stream1 += *stream2;
    QVERIFY(stream1->time() == 0.0);
    QVERIFY(stream1->oilRate() == newOilRate);
    QVERIFY(stream1->gasRate() == newGasRate);
    QVERIFY(stream1->waterRate() == newWaterRate);
    QVERIFY(stream1->pressure() == 0.0);
    QVERIFY(stream1->inputUnits() == Stream::FIELD);
}

void TestStream::testPlusOperator()
{
    const Stream stream3 = *stream1 + *stream2;
    QVERIFY(stream3.time() == stream2->time());
    QVERIFY(stream3.inputUnits() == Stream::FIELD);
    QVERIFY(stream3.pressure() == 0);
    QVERIFY(stream3.oilRate() == stream1->oilRate(stream3.inputUnits()) + stream2->oilRate(stream3.inputUnits()));
    QVERIFY(stream3.gasRate() == stream1->gasRate(stream3.inputUnits()) + stream2->gasRate(stream3.inputUnits()));
    QVERIFY(stream3.waterRate() == stream1->waterRate(stream3.inputUnits()) + stream2->waterRate(stream3.inputUnits()));
}

void TestStream::testMultiplicationOperator()
{
    const double fac = 2.0;
    const Stream stream3 = static_cast<const Stream>(*stream2 ) * fac;
    QVERIFY(stream3.time() == 0.0);
    QVERIFY(stream3.oilRate() == 20.0);
    QVERIFY(stream3.gasRate() == 40.0);
    QVERIFY(stream3.waterRate() == 60.0);
    QVERIFY(stream3.pressure() == 100.0);
//    QVERIFY(stream3->inputUnits() == Stream::FIELD);
    stream3.printToCout();
}

void TestStream::testAverage()
{
    QVector<Stream*> vec;
    vec.append(new Stream());
    vec.append(new Stream(1, 100, 200, 300, 500));
    vec.append(new Stream(2, 200, 400, 600, 1000));
    Stream* avg_stream = new Stream;
    avg_stream->avg(vec, 0);

    QVERIFY(avg_stream->time() == 2.0);
    QVERIFY(avg_stream->oilRate() == 150.0);
    QVERIFY(avg_stream->gasRate() == 300.0);
    QVERIFY(avg_stream->waterRate() == 450.0);
    QVERIFY(avg_stream->pressure() == 750.0);
    QVERIFY(avg_stream->inputUnits() == Stream::FIELD);
}

void TestStream::testPrinter()
{
    static_cast<const Stream*>(stream2)->printToCout();
}

void TestStream::cleanupTestCase()
{
}
