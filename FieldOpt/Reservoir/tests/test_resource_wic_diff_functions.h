//
// Created by bellout on 8/4/16.
//
#ifndef FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
#define FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H


//#include "Reservoir/tests/test_resource_wic_widata.h"
#include "test_resource_wic_widata.h"
#include <Eigen/Dense>

using namespace Eigen;

namespace TestResources {
    namespace WIBenchmark {

/*!
 * \brief
 * \param
 * \return
 */
        bool DiffVectorLength(WIData va, WIData vb);

/*!
 * \brief
 * \param
 * \return
 */
        bool CompareIJK(WIData va, WIData vb);
    }
}
#endif //FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
