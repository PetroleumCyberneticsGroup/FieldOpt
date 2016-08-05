//
// Created by bellout on 8/4/16.
//
#ifndef FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
#define FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H

#include "Reservoir/tests/test_resource_wic_widata.h"
#include <Eigen/Dense>
#include <typeinfo>

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

        template <typename T> T incr( T& a, T b = 1 ) { return a += b; }

        template <typename TTT> TTT CheckTagTest(WIData& va, WIData& vb, WIData& vdiff, QString& tag){
            return vdiff;
        }

        void CheckTag(WIData va, WIData vb, WIData vdiff, QString tag){

            // Default def before being properly defined within if-loop
            Matrix<double, Dynamic, Dynamic> vdiffLocal;

            if(tag=="IJK"){
                auto vdiffLocal = vdiff.IJK;
                std::cout << "Testing: IJK >> "; // Confirmation
            }else if(tag=="WCF"){
                auto vdiffLocal = vdiff.WCF;
                std::cout << "Testing: WCF >> ";
            }else {
                std::cout << "Specify tag!" << std::endl;
            }

            auto *vdiffLocal_ptr = &vdiffLocal;
        }
        
/*!
 * \brief
 * \param
 * \return
 */
        void CheckRowwiseDiff(WIData va, WIData vb, WIData vdiff, QString tag){

            // Default def before being properly defined within if-loop
            Matrix<double, Dynamic, Dynamic> vdiffLocal;
//            *vdiffLocal_ptr = &vdiffLocal;

            if(tag=="IJK"){
				auto vdiffLocal = vdiff.IJK;
                std::cout << "Testing: IJK >> "; // Confirmation
			}else if(tag=="WCF"){
				auto vdiffLocal = vdiff.WCF;
                std::cout << "Testing: WCF >> ";
			}else {
                std::cout << "Specify tag!" << std::endl;
            }

            Matrix<int,1,1> one;
            Matrix<int,1,1> two;
            one << 1;
            two << 2;
//            int one = 1;
//            int two = 2;
//            double one = 1;
//            double two = 2;
            auto test = incr(one,two);
            std::cout << "Testing!" << test << "which is of type: " << typeid(test).name() << std::endl;

            auto testTTT = CheckTagTest(va, vb, vdiff, tag);
            std::cout << "Testing!" << testTTT << "which is of type: " << typeid(testTTT).name() << std::endl;

//            std::cout << "Testing!" << &vdiffLocal_ptr << std::endl;

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
        void CheckColumnwiseDiff(WIData va, WIData vb, WIData vdiff, QString tag){

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
                QString tag = "IJK";
                CheckRowwiseDiff(va,vb,vdiff,tag);

                // Output difference for individual columns (later)
                // Produce quantitative differences, e.g., using percentages:
                // %xx of K column values are equal
                std::cout << "Columnwise differences are:" << std::endl;
            }

            bool debug_ = true;
            if (debug_){
                std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl
                          << "(WIDataPCG.IJK - WIDataPCG.IJK)= " << std::endl
	                      << vdiff.IJK.block(0,0,10,4)
	                      << std::endl << "..." << std::endl;
                std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;                          
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
            QString tag = "WCF";
            CheckRowwiseDiff(va,vb,vdiff,tag);

            return vdiff;
        }



    }
}
#endif //FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
