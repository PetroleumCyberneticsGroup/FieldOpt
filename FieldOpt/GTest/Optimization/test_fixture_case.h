/******************************************************************************
 *
 *
 *
 * Created: 30.11.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
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
 *****************************************************************************/

#ifndef TEST_FIXTURE_CASE
#define TEST_FIXTURE_CASE

#include "Optimization/case.h"
#include "GTest/Optimization/test_fixture_optimizer.h"
#include <QList>

class CaseTestFixture : public OptimizerTestFixture {
protected:
    CaseTestFixture() {
        /* Case 1:
         * Only integer variables.
         * Objective function not evaluated.
         */
        cases_.append(new Optimization::Case(QHash<QUuid,bool>(), integer_variables_, QHash<QUuid,double>()));

        /* Case 2:
         * Only real variables.
         * Objective function evaluated.
         */
        cases_.append(new Optimization::Case(QHash<QUuid,bool>(), QHash<QUuid,int>(), real_variables_));
        cases_.last()->set_objective_function_value(100.0);

        /* Case 3:
         * All variable types.
         * Objective function evaluated.
         */
        cases_.append(new Optimization::Case(binary_variables_, integer_variables_, real_variables_));
        cases_.last()->set_objective_function_value(-50.0);

        /* Case 4:
         * Identical to case 3.
         */
        cases_.append(new Optimization::Case(binary_variables_, integer_variables_, real_variables_));
        cases_.last()->set_objective_function_value(-50.0);
    }
    QList<Optimization::Case *> cases_;
    const QHash<QUuid, bool> binary_variables_ {
        {QUuid::createUuid(), true},
        {QUuid::createUuid(), true},
        {QUuid::createUuid(), false},
        {QUuid::createUuid(), false}
    };
    const QHash<QUuid, int> integer_variables_ {
        {QUuid::createUuid(), 1},
        {QUuid::createUuid(), 2},
        {QUuid::createUuid(), 5}
    };
    const QHash<QUuid, double> real_variables_ {
        {QUuid::createUuid(), 1.0},
        {QUuid::createUuid(), 4.0},
        {QUuid::createUuid(), 2.5}
    };
};

#endif // TEST_FIXTURE_CASE
