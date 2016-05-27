#ifndef FIELDOPT_TEST_RESOURCE_CASES_H
#define FIELDOPT_TEST_RESOURCE_CASES_H

#include "Optimization/case.h"
#include <QHash>
#include <QUuid>

class TestResourceCases {
public:
    TestResourceCases(){
        test_case_1_ = new Optimization::Case(QHash<QUuid,bool>(), integer_variables_, QHash<QUuid,double>());
        test_case_2_ = new Optimization::Case(QHash<QUuid,bool>(), QHash<QUuid,int>(), real_variables_);
        test_case_2_->set_objective_function_value(100.0);
        test_case_3_ = new Optimization::Case(binary_variables_, integer_variables_, real_variables_);
        test_case_3_->set_objective_function_value(-50.0);
        test_case_4_ = new Optimization::Case(binary_variables_, integer_variables_, real_variables_);
        test_case_4_->set_objective_function_value(-50.0);
    }

    /* Case 1:
     * Only integer variables.
     * OBjective function not evaluated.
     */
    Optimization::Case *test_case_1_;

    /* Case 2:
     * Only real variables.
     * Objective function evaluated.
     */
    Optimization::Case *test_case_2_;

    /* Case 3:
     * All variable types.
     * Objective function evaluated.
     */
    Optimization::Case *test_case_3_;

    /* Case 4:
     * Identical to case 3.
     */
    Optimization::Case *test_case_4_;

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

#endif //FIELDOPT_TEST_RESOURCE_CASES_H
