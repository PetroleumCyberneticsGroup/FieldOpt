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

#include "testconstraint.h"

void TestConstraint::initTestCase()
{
    constraint1 = new Constraint;
    constraint2 = new Constraint(5.0, 2.0, 5.0);
}

void TestConstraint::testId()
{
    QVERIFY(constraint1->id() == 0);
    QVERIFY(constraint2->id() == 1);
}

void TestConstraint::testName()
{
    // Test default names
    QVERIFY(constraint1->name() == "unknown");
    QVERIFY(constraint2->name() == "unknown");

    // Test set names
    constraint1->setName("constraint_1");
    constraint2->setName("constraint_2");
    QVERIFY(constraint1->name() == "constraint_1");
    QVERIFY(constraint2->name() == "constraint_2");
}

void TestConstraint::testValue()
{
    // Test default/initialized values
    QVERIFY(constraint1->value() == 0.0);
    QVERIFY(constraint2->value() == 5.0);

    // Test set values
    constraint1->setValue(1.0);
    constraint2->setValue(4.0);
    QVERIFY(constraint1->value() == 1.0);
    QVERIFY(constraint2->value() == 4.0);
}

void TestConstraint::cleanupTestCase()
{
}
