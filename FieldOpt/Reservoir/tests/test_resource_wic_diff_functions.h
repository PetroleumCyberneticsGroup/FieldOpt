//
// Created by bellout on 8/4/16.
//
#ifndef FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
#define FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H

#include "Reservoir/tests/test_resource_wic_widata.h"
#include <Eigen/Dense>
#include <typeinfo>
#include <iomanip>

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
        double GetColumnAccuracyElements(Matrix<double,Dynamic,1> col_vector){

			// accuracy_elements: fraction of elements in column which are zero up to given tolerance
			double nrows = col_vector.rows();
			double nrows_nz = 0;

            for( int ii=0; ii < nrows; ++ii ) {
            	Matrix<double,1,1> row_element;
            	row_element << col_vector[ii];
				if(row_element.isZero(GetEps())){ nrows_nz += 1; }
			}

			double accuracy_elements = nrows_nz / nrows;
			return accuracy_elements;
        }

/*!
 * \brief
 * \param
 * \return
 */
        double GetColumnOffset(Matrix<double,Dynamic,1> col_vector){

			// accuracy_magniture: norm of difference vector
			double column_offset = col_vector.norm();
			return column_offset;
        }

/*!
 * \brief
 * \param
 * \return
 */
        QList<Matrix<double,1,4>> CheckColumnwiseDiff(WIData va, WIData vb, WIData vdiff){

			// Output msg
        	auto vdiff_ = vdiff.IJK.cast<double>();

			Matrix<double,1,4> IJK_accuracy_elements;
			Matrix<double,1,4> IJK_column_offset;

            for( int ii=0; ii < vdiff_.cols(); ++ii ) {
                IJK_accuracy_elements(ii) = GetColumnAccuracyElements(vdiff_.col(ii));
				IJK_column_offset(ii) = GetColumnOffset(vdiff_.col(ii));
            }

			QList<Matrix<double,1,4>> IJK_accuracy_list;
			IJK_accuracy_list.append(IJK_accuracy_elements);
			IJK_accuracy_list.append(IJK_column_offset);

            // Accuracy
            std::cout << "Element column accuracy: fraction of elements in difference "
                         "columns that are zero up to given tolerance" << std::endl;
			std::cout << "Element accuracy I column:  "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[0][0] << std::endl;
			std::cout << "Element accuracy J column:  "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[0][1] << std::endl;
			std::cout << "Element accuracy K1 column: "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[0][2] << std::endl;
			std::cout << "Element accuracy K2 column: "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[0][3] << std::endl;
			// Offset
			std::cout << "Column IJK offset: norm of difference vector" << std::endl;
			std::cout << "Column offset I column:  "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[1][0] << std::endl;
			std::cout << "Column offset J column:  "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[1][1] << std::endl;
			std::cout << "Column offset K1 column: "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[1][2] << std::endl;
			std::cout << "Column offset K2 column: "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[1][3] << std::endl;

			return IJK_accuracy_list;
        }

/*!
 * \brief
 * \param
 * \return
 */
        template<typename T, typename V> void CheckRowwiseDiff(T& va_, T& vb_, V& vdiff_, string tag){

            // Check vector has length > 0
            if (!vdiff_.rows()>0)
                throw std::runtime_error("Difference vector (vdiff_) has no elements");

			// Output msg
            std::cout << "Testing: " << tag << " (rowwise) >> "
                      << "values differ at the following rows:" << std::endl;
            for( int ii=0; ii < vdiff_.rows(); ++ii ) {
                auto row = vdiff_.row(ii);

                if (!row.isZero(GetEps())){
                    std::cout << "row " << ii << ":"
                              << " RMS=" << va_.row(ii)
                              << " PCG=" << vb_.row(ii)
                              << " DF=" << std::fixed << std::setw(7) << vdiff_(ii,0) << std::endl;
                    // TO DO: COLORED DIFFERENCES
                    // QString str_out;
                    // QString str_out = QString::number(row(ii));
                    // for (int ii; ii < row.size(); ++ii){
                    //     if (fabs(row(ii))<1e-3){
                    //         str_out.append("0");
                    //     }else{
                    //         str_out.append("\033[1;31m" + QString::number(row(ii)) + "\033[0m");
                    //     }
                    // }
                    // std::cout << str_out.toStdString() << std::endl;
                }
            }
	    }

/*!
 * \brief
 * \param
 * \return
 */
        void CheckRowwiseDiffIJK(WIData va, WIData vb, WIData vdiff){
        	auto vdiff_ = vdiff.IJK;
        	auto va_ = va.IJK;
        	auto vb_ = vb.IJK;
			CheckRowwiseDiff(va_, vb_, vdiff_, "IJK");
        }

/*!
 * \brief
 * \param
 * \return
 */
        void CheckRowwiseDiffWCF(WIData va, WIData vb, WIData vdiff){
        	auto vdiff_ = vdiff.WCF;
        	auto va_ = va.WCF;
        	auto vb_ = vb.WCF;
			CheckRowwiseDiff(va_, vb_, vdiff_, "WCF");
        }

/*!
 * \brief
 * \param
 * \return
 */
        WIData CompareIJK(WIData va, WIData vb){

		    WIData vdiff;
		    vdiff.IJK = va.IJK - vb.IJK;
			QList<Matrix<double,1,4>> IJK_accuracy_list;

			// Check if IJK values computed by RMS and PCG WIC are equal
			// If not, output differing rows
            if (vdiff.IJK.isZero(GetEps())){
                std::cout << "IJK values match exactly for this well." << std::endl;
            }else{
                std::cout << "IJK values are NOT the same for this well." << std::endl;

                // Output row differences (i.e., I, J and K columns combined)
                CheckRowwiseDiffIJK(va,vb,vdiff);

                // Output difference for individual columns
                IJK_accuracy_list = CheckColumnwiseDiff(va,vb,vdiff);
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
            double IJK_accuracy_elements;
            double IJK_column_offset;

            bool test = va.WCF.isApprox(vb.WCF, 1e-2);

            // Output general difference (i.e., for I, J and K columns)
            CheckRowwiseDiffWCF(va,vb,vdiff);

            // Output difference for column
            IJK_accuracy_elements = GetColumnAccuracyElements(vdiff.WCF);
            IJK_column_offset = GetColumnOffset(vdiff.WCF);

            QList<double> WCF_accuracy_list;
            WCF_accuracy_list.append(IJK_accuracy_elements);
            WCF_accuracy_list.append(IJK_column_offset);

            std::cout << "Element accuracy: fraction of elements in difference "
                         "column that are zero up to given tolerance" << std::endl;
            std::cout << "Element accuracy:  "
            << std::fixed << std::setprecision(4) << WCF_accuracy_list[0] << std::endl;
            // Magnitude accuracies
            std::cout << "Column IJK offset: norm of difference vector" << std::endl;
            std::cout << "Column offset:  "
            << std::fixed << std::setprecision(4) << WCF_accuracy_list[1] << std::endl;

            bool debug_ = true;
            if (debug_){
                std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl
                          << "(WIDataPCG.WCF - WIDataPCG.WCF)= " << std::endl
	                      << vdiff.WCF.block(0,0,10,1)
	                      << std::endl << "..." << std::endl;
                std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
            }

            return vdiff;
        }



    }
}
#endif //FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
