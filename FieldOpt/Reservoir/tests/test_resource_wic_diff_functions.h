//
// Created by bellout on 8/4/16.
//
#ifndef FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
#define FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H

#include "Reservoir/tests/test_resource_wic_widata.h"
#include <Eigen/Dense>
#include <typeinfo>

#include <iomanip>
#include <QProcess>
#include <QVector>

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
        WIData GetLongestVector(WIData va, WIData vb){

            if (va.IJK.rows() > vb.IJK.rows()){
                return va;
            }else{
                return vb;
            }
        }

/*!
 * \brief
 * \param
 * \return
 */
        WIData GetShortestVector(WIData va, WIData vb){

            if (va.IJK.rows() < vb.IJK.rows()){
                return va;
            }else{
                return vb;
            }
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
            double GetColumnOffset(Matrix<double,Dynamic,1> va,
                                   Matrix<double,Dynamic,1> vb,
                                   Matrix<double,Dynamic,1> vdiff){

			// accuracy_magniture: norm of difference vector
            double column_offset = vdiff.norm();

            // return
			return column_offset;
        }

/*!
 * \brief
 * \param
 * \return
 */
            double GetColumnCosine(Matrix<double,Dynamic,1> va,
                                   Matrix<double,Dynamic,1> vb,
                                   Matrix<double,Dynamic,1> vdiff){

            // cosine similarity
            double cosine_measure = va.dot(vb) / (va.norm() * vb.norm());

            // return
            return cosine_measure;
        }

/*!
 * \brief
 * \param
 * \return
 */
        QList<Matrix<double,1,4>> CheckColumnwiseDiff(WIData va, WIData vb, WIData vdiff){

			//
        	auto va_ = va.IJK.cast<double>();
            auto vb_ = vb.IJK.cast<double>();
            auto vdiff_ = vdiff.IJK.cast<double>();

			Matrix<double,1,4> IJK_accuracy_elements;
			Matrix<double,1,4> IJK_column_offset;
            Matrix<double,1,4> IJK_column_cosine;

            for( int ii=0; ii < vdiff_.cols(); ++ii ) {
                IJK_accuracy_elements(ii) = GetColumnAccuracyElements(vdiff_.col(ii));
				IJK_column_offset(ii) = GetColumnOffset(va_.col(ii), vb_.col(ii), vdiff_.col(ii));
                IJK_column_cosine(ii) = GetColumnCosine(va_.col(ii), vb_.col(ii), vdiff_.col(ii));
            }

			QList<Matrix<double,1,4>> IJK_accuracy_list;
			IJK_accuracy_list.append(IJK_accuracy_elements);
			IJK_accuracy_list.append(IJK_column_offset);
            IJK_accuracy_list.append(IJK_column_cosine);

            // Zero element fraction
            std::cout << "\033[1;33mElement accuracy: fraction of zero (<tol) elements in diff. "
                         "column (1=best)\033[0m" << std::endl;
			std::cout << "Element accuracy I column:  "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[0][0] << std::endl;
			std::cout << "Element accuracy J column:  "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[0][1] << std::endl;
			std::cout << "Element accuracy K1 column: "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[0][2] << std::endl;
			std::cout << "Element accuracy K2 column: "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[0][3] << std::endl;
            // Column offset
            std::cout << "\033[1;33mColumn IJK offset: norm of diff. vector [Euclidean dist.]  (0=best)\033[0m" << std::endl;
			std::cout << "Column offset I column:  "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[1][0] << std::endl;
			std::cout << "Column offset J column:  "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[1][1] << std::endl;
			std::cout << "Column offset K1 column: "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[1][2] << std::endl;
			std::cout << "Column offset K2 column: "
			<< std::fixed << std::setprecision(4) << IJK_accuracy_list[1][3] << std::endl;
            // Cosine measure
            std::cout << "\033[1;33mColumn IJK cosine measure: angle b/e vectors (1=parallel=best)\033[0m" << std::endl;
            std::cout << "Column cosine I column:  "
            << std::fixed << std::setprecision(8) << IJK_accuracy_list[2][0] << std::endl;
            std::cout << "Column cosine J column:  "
            << std::fixed << std::setprecision(8) << IJK_accuracy_list[2][1] << std::endl;
            std::cout << "Column cosine K1 column: "
            << std::fixed << std::setprecision(8) << IJK_accuracy_list[2][2] << std::endl;
            std::cout << "Column cosine K2 column: "
            << std::fixed << std::setprecision(8) << IJK_accuracy_list[2][3] << std::endl;

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
            std::cout << "\033[1;33mTesting: " << tag << " (rowwise) >> "
                      << "values differ at the following rows:\033[0m" << std::endl;
            for( int ii=0; ii < vdiff_.rows(); ++ii ) {
                auto row = vdiff_.row(ii);

                if (!row.isZero(GetEps())){

                    IOFormat CleanFmt(4, 0, " ", "\n", "", "");
                    std::cout << "row "  << std::setw(3) << ii << ":"  // << std::setprecision(4)
                              << std::setw(4) << " RMS=" << std::setw(1) << va_.row(ii).format(CleanFmt)
                              << std::setw(4) << " PCG=" << std::setw(1) << vb_.row(ii).format(CleanFmt)
                              << std::setw(4) << " DF="  << std::setw(1) << vdiff_.row(ii).format(CleanFmt) << std::endl;

                              // << " DF=" << std::fixed << std::setw(7) << vdiff_(ii,0) << std::endl;
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
                std::cout << "\033[1;32mIJK values match exactly for this well.\033[0m" << std::endl;
            }else{
                std::cout << "\033[1;35mIJK values are NOT the same for this well.\033[0m" << std::endl;
                // Output row differences (i.e., I, J and K columns combined)
                CheckRowwiseDiffIJK(va,vb,vdiff);
                // Output difference for individual columns
                IJK_accuracy_list = CheckColumnwiseDiff(va,vb,vdiff);
            }

            bool debug_ = false;
            if (debug_){
                int nrows = (vdiff.IJK.rows() > 10) ? 10 : vdiff.IJK.rows();
                std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl
                          << "(WIDataPCG.IJK - WIDataPCG.IJK)= " << std::endl
	                      << vdiff.IJK.block(0,0,nrows,4)
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
            QList<double> WCF_accuracy_list;

            if(va.WCF.isApprox(vb.WCF, 1e-3)){
                std::cout << "\033[1;32mWCF values match exactly for this well.\033[0m" << std::endl;
            }else{
                std::cout << "\033[1;35mWCF values are NOT the same for this well.\033[0m" << std::endl;

                // Output general difference (i.e., for I, J and K columns)
                CheckRowwiseDiffWCF(va,vb,vdiff);

                // Output difference for column
                WCF_accuracy_list.append(GetColumnAccuracyElements(vdiff.WCF));
                WCF_accuracy_list.append(GetColumnOffset(va.WCF, vb.WCF, vdiff.WCF));
                WCF_accuracy_list.append(GetColumnCosine(va.WCF, vb.WCF, vdiff.WCF));

                // Zero element fraction
                std::cout << "\033[1;33mElement accuracy: fraction of zero (<tol) elements in diff. "
                             "column (1=best)\033[0m" << std::endl;
                std::cout << "Element accuracy:  "
                << std::fixed << std::setprecision(4) << WCF_accuracy_list[0] << std::endl;
                // Column offset
                std::cout << "\033[1;33mColumn WCF offset: norm of difference vector (0=best)\033[0m" << std::endl;
                std::cout << "Column offset:  "
                << std::fixed << std::setprecision(4) << WCF_accuracy_list[1] << std::endl;
                // Cosine measure
                std::cout << "\033[1;33mColumn WCF cosine measure: angle b/e vectors (1=parallel=best)\033[0m" << std::endl;
                std::cout << "Column cosine measure:  "
                << std::fixed << std::setprecision(8) << WCF_accuracy_list[2] << std::endl;
            }

            bool debug_ = false;
            if (debug_){
                int nrows = (vdiff.WCF.rows() > 10) ? 10 : vdiff.WCF.rows();
                std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl
                          << "(WIDataPCG.WCF - WIDataPCG.WCF)= " << std::endl
	                      << vdiff.WCF.block(0,0,nrows,1)
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
 // QList<WIData> RemoveSuperfluousRows(&WIDataRMS_PCG,&diff_files){
        void RemoveSuperfluousRows(WIData &WIDataRMS, WIData &WIDataPCG, QStringList &diff_files){

            bool debug_ = false;

            // DIFF TREATMENT: IJK COMPARISON: FIND EXTRA ROWS USING diff COMMAND
            QProcess diff_process, grep_process;
            diff_process.setStandardOutputProcess(&grep_process);
            grep_process.setProcessChannelMode(QProcess::MergedChannels);

            if (debug_) {
                std::cout << "Original number of rows in RMS and PCG data:" << std::endl;
                std::cout << "WIDataPCG.IJK.rows:" << WIDataPCG.IJK.rows() << std::endl;
                std::cout << "WIDataRMS.IJK.rows:" << WIDataRMS.IJK.rows() << std::endl;
            }

            // CALL diff COMMAND USING QProcess; PIPE IT TO grep COMMAND
            // SWITCH COLUMNS SUCH THAT LONGEST COLUMN IS ALWAYS THE COLUMN TO THE RIGHT
            if (WIDataRMS.IJK.rows() > WIDataPCG.IJK.rows()) { // longer vector = right column
                diff_process.start("diff -y " + diff_files[1] + " " + diff_files[0]); // PCG[1] < RMS[0]
            } else {
                diff_process.start("diff -y " + diff_files[0] + " " + diff_files[1]); // RMS[0] < PCG[1]
            }
            grep_process.start("grep \">\" -n");
            diff_process.waitForFinished();
            grep_process.waitForFinished();

            // READ OUTPUT FROM QProcess COMMAND + CLOSE PROCESSES
            QByteArray grep_output = grep_process.readAllStandardOutput();
            diff_process.close();
            grep_process.close();

            // OBTAIN INDICES OF SUPERFLUOUS ROWS IN LONGEST COLUMN:
            // SPLIT QString SUCH THAT EACH LINE IS ONE ELEMENT IN A QStringList
            QString DataAsString = QString::fromLatin1(grep_output.data());
            QStringList textString = DataAsString.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
            QVector<int> sup_indices;

            // OBTAIN INDICES OF SUPERFLUOUS ROWS IN LONGEST COLUMN:
            // READ EACH LINE OF diff OUTPUT AND STORE INDICES -- IMPORTANT: INDICES OBTAINED
            // FROM DIFF START FROM 1; WE THEREFORE SUBSTRACT 1 FROM THESE TO MAKE THESE WORK
            // WITH 0-START INDEXING
            if (debug_) std::cout << std::endl << "superfluous indices in rightmost column:" << std::endl;
            for (int ii = 0; ii < textString.size(); ++ii) {
                sup_indices.append(textString[ii].split(":").first().toInt() - 1); // CONVERT TO 0-START INDEXING
                if (debug_) std::cout << ii << ":" << sup_indices[ii] << std::endl;
            }
            if (debug_) std::cout << "sup_indices.size:" << sup_indices.size() << std::endl;

            // REMOVE SUPERFLUOUS ROWS
            WIData WILong = GetLongestVector(WIDataRMS, WIDataPCG);
            WIData WIShort = GetShortestVector(WIDataRMS, WIDataPCG);

            WIShort.IJK.setZero();
            std::cout << "WILong.IJK.rows:" << WILong.IJK.rows() << std::endl;
            std::cout << "WIShort.IJK.rows:" << WIShort.IJK.rows() << std::endl;

            // LOOP OVER ALL ROWS IN THE LONGEST COLUMN AND INSERT EACH OF THESE INTO A NEW IJK
            // COLUMN UNLESS THE GIVEN ROW IS A SUPERFLUOUS ONE, IN WHICH CASE WE SKIP IT
            int kk = 0;
            for (int ii = 0; ii < WILong.IJK.rows(); ++ii) {
                if (debug_) std::cout << "kk: " << kk << std::endl;
                if (!sup_indices.contains(ii)) {
                    WIShort.IJK.row(kk) << WILong.IJK.row(ii);
                    WIShort.WCF.row(kk) << WILong.WCF.row(ii);
                }
                kk += 1;
            }

            // MAKE ORIGINALLY (TOO-)LONG COLUMN EQUAL TO COLUMN WITHOUT SUPERFLUOUS ROWS
            if (WIDataRMS.IJK.rows() > WIDataPCG.IJK.rows()) {
                WIDataRMS.IJK = WIShort.IJK;
                WIDataRMS.WCF = WIShort.WCF;
            } else {
                WIDataPCG.IJK = WIShort.IJK;
                WIDataPCG.WCF = WIShort.WCF;
            }

            if (debug_){
                std::cout << "WIDataRMS.IJK.rows:" << WIDataRMS.IJK.rows() << std::endl;
                std::cout << "WIDataPCG.IJK.rows:" << WIDataPCG.IJK.rows() << std::endl;
            }

            // VECTOR LENGTHS HAVE BEE MADE EQUAL => COMPARE DIRECTLY
            std::cout << "\033[1;36m" << WIDataRMS.dir_name.toStdString() <<
                      ": >>> Vector lengths have been made equal. Making comparison.\033[0m" <<
                      std::setfill(' ') << std::endl;

        }

    }
}
#endif //FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
