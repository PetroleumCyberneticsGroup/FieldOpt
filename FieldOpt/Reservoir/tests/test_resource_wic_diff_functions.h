//
// Created by bellout on 8/4/16.
//
#ifndef FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
#define FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H

#include "Reservoir/tests/test_resource_wic_widata.h"
#include <Eigen/Dense>

using namespace Eigen;

namespace TestResources {
    namespace WIBenchmark {

/*!
 * \brief
 * \param
 * \return
 */
        bool DiffVectorLength(WIData va, WIData vb){

			bool vector_diff = true;
			if (va.IJK.rows() != vb.IJK.rows()){
			    vector_diff = false;
			}
			return vector_diff;
		}

/*!
 * \brief
 * \param
 * \return
 */
        bool CompareIJK(WIData va, WIData vb){

		    auto vdiff = va.IJK - vb.IJK;

		    std::cout << "WIDataPCG.IJK " << vdiff << std::endl;

		    return true;
		}

    }
}
#endif //FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
