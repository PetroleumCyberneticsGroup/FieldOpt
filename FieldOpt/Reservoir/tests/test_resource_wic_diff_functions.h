//
// Created by bellout on 8/4/16.
//
#ifndef FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
#define FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H

#include <Eigen/Dense>

using namespace Eigen;

namespace TestResources {
    namespace DiffFunctions {

/*!
 * \brief
 * \param
 * \return
 */
        bool DiffVectorLength(Matrix<double,Dynamic,1> va, Matrix<double,Dynamic,1> vb);

/*!
 * \brief
 * \param
 * \return
 */

    }
}
#endif //FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
