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
        double GetEps(){
            double eps = 1e-16;
            return eps;
        }
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
        void CheckRowwiseDiff(WIData va, WIData vb, WIData vdiff, string DiffFlag){

            if(DiffFlag.compare("IJK")){
				auto vdiffLocal = vdiff.IJK;
			}else if(DiffFlag.compare("WCF")){
				auto vdiffLocal = vdiff.WCF;
			}else {
                std::cout << "Specify flag!" << std::endl;
            }
            std::cout << "Testing: " << DiffFlag << " >> ";
            
            std::cout << "the values differ at the following rows:" << std::endl;
            for( int ii=0; ii < vdiffLocal.rows(); ++ii ) {
                auto IJKrow = vdiffLocal.row(ii);
                if (!IJKrow.isZero(GetEps())){
                    std::cout << "row " << ii << ":"
                              << " RMS=" << va.IJK.row(ii)
                              << " PCG=" << vb.IJK.row(ii) << std::endl;
                }
            }
        }

/*!
 * \brief
 * \param
 * \return
 */
        WIData CompareIJK(WIData va, WIData vb){

		    WIData vdiff;
		    vdiff.IJK = va.IJK - vb.IJK;

			// Check if IJK values computed by RMS and PCG WIC are equal
			// If not, output differing rows  
            if (vdiff.IJK.isZero(GetEps())){
                std::cout << "IJK values match exactly for this well." << std::endl;
            }else{
                std::cout << "IJK values are NOT the same for this well." << std::endl;

                // Output general difference (i.e., for I, J and K columns)
                CheckRowwiseDiff(va,vb,vdiff,"IJK");

                // Output difference for individual columns (later)
                // Produce quantitative differences, e.g., using percentages:
                // %xx of K column values are equal
                std::cout << "Columnwise differences are:" << std::endl;
            }

            bool debug_ = true;
            if (debug_){
			    std::cout << "\033[1;31m<---DEBUG--->\033[0m" << std::endl
                          << "(WIDataPCG.IJK - WIDataPCG.IJK)= " << std::endl
	                      << vdiff.IJK.block(0,0,10,4)
	                      << std::endl << "..." << std::endl;
            }

		    return vdiff;
		}

/*!
 * \brief
 * \param
 * \return
 */
        WIData CompareWCF(WIData va, WIData vb) {

            WIData vdiff;
            vdiff.WCF = va.WCF - vb.WCF;

            bool test = va.WCF.isApprox(vb.WCF, 1e-2);

            // Output general difference (i.e., for I, J and K columns)
            CheckRowwiseDiff(va,vb,vdiff,"WCF");

            return vdiff;
        }



    }
}
#endif //FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
