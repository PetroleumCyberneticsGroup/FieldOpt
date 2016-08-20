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
        double GetEpsIJK(){
            double eps = 1e-12; // tolerance for comparison, NOT for removing rows
            return eps;
        }

/*!
 * \brief
 * \param
 * \return
 */
        double GetEpsWCF(){
            double eps = 0.05; // tolerance for comparison, NOT for removing rows
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
        void RemoveRowsLowWCF(WIData &data){

            int nRows;
            double tol = .01; // tolerance for removing row
            int nColsWCF  = data.WCF.cols();
            int nColsIJK  = data.IJK.cols();

            int rem_count = 0;
            QString str_out, temp_str, msg, nl, lstr_out;
            int max_counter = 0;

            lstr_out = "\n--------------------------------------------------------------------------------";
            str_out = lstr_out + "\n>>> If any, start removing rows with low WCF for "
                      + data.data_tag + " data.";

            for( int ii=0; ii < data.WCF.rows(); ++ii ) {

                auto wcf_num = data.WCF.row(ii).value();
                QString wcf_str;
                wcf_str.sprintf("%5.3f", wcf_num);

                if (wcf_num < tol){

                    if (wcf_num > 0){
                        msg = "is lower than set tolerance";
                    }else{
                        msg = "is negative!";
                        max_counter += 1;
                    }
                    if (max_counter <= 10){

                        temp_str = "Removing row " + QString::number(ii)
                                   + " from " + data.data_tag
                                   + " data b/c WCF " + msg + " ("
                                   + wcf_str + " < "
                                   + QString::number(tol) + ")";
                        // nl = (rem_count > 0) ? "\n" : "";
                        str_out.append("\n" + temp_str);
                    }

                    nRows = data.WCF.rows()-1;
                    // WCF
                    data.WCF.block(ii,0,nRows-ii,nColsWCF) = data.WCF.block(ii+1,0,nRows-ii,nColsWCF);
                    data.WCF.conservativeResize(nRows,nColsWCF);
                    // IJK
                    data.IJK.block(ii,0,nRows-ii,nColsIJK) = data.IJK.block(ii+1,0,nRows-ii,nColsIJK);
                    data.IJK.conservativeResize(nRows,nColsIJK);

                    rem_count += 1;
                }
            }
            nl = (rem_count > 0) ? "\n" : "";
            str_out.append(nl);

            if (max_counter > 10){
                str_out = str_out + "+" + QString::number(max_counter)
                          + " other rows removed b/c WCF " + msg + "\n";
            }
            // str_out = str_out + ">>> Finished removing rows with low WCF for "
            //           + data.data_tag + " data.";
            if (rem_count < 1) {str_out = str_out + " [None removed.]";}

            std::cout << std::endl << "\033[1;33m" << str_out.toStdString() << "\033[0m";
            Utilities::FileHandling::WriteLineToFile(str_out, data.tex_file);
        }

/*!
 * \brief
 * \param
 * \return
 */
        double GetColumnAccuracyElements(Matrix<double,Dynamic,1> col_vector){

			// accuracy_elements: fraction of elements in column which
            //are zero up to given tolerance
			double nrows = col_vector.rows();
			double nrows_nz = 0;

            for( int ii=0; ii < nrows; ++ii ) {
            	Matrix<double,1,1> row_element;
            	row_element << col_vector[ii];
				if(row_element.isZero(GetEpsWCF())){ nrows_nz += 1; }
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

			// accuracy_magnitude: norm of difference vector
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

            QString temp_str, str_out, nl;
            QString lstr_out = "\n--------------------------------------------------------------------------------";

            // Zero element fraction
            str_out = "\nElement accuracy: fraction of zero (<tol) elements in diff. column (1=best)";
            std::cout << "\033[1;33m" << str_out.toStdString() << "\033[0m" << std::endl;
            Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

            str_out  = "Element accuracy I column: " + QString::number(IJK_accuracy_list[0][0]);
            temp_str = "Element accuracy J column: " + QString::number(IJK_accuracy_list[0][1]);
            str_out.append("\n" + temp_str);
            temp_str = "Element accuracy K column: " + QString::number(IJK_accuracy_list[0][2]);
            str_out.append("\n" + temp_str);
//            temp_str = "Element accuracy K2 column: " + QString::number(IJK_accuracy_list[0][3]);
//            str_out.append("\n" + temp_str);
            std::cout << str_out.toStdString() << std::endl;
            Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

            // Column offset
            str_out = "Column IJK offset: norm of diff. vector [Euclidean dist.]  (0=best)";
            std::cout << "\033[1;33m" << str_out.toStdString() << "\033[0m" << std::endl;
            Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

            str_out  = "Column offset I column:  " + QString::number(IJK_accuracy_list[1][0]);
            temp_str = "Column offset J column:  " + QString::number(IJK_accuracy_list[1][1]);
            str_out.append("\n" + temp_str);
            temp_str = "Column offset K column:  " + QString::number(IJK_accuracy_list[1][2]);
            str_out.append("\n" + temp_str);
//            temp_str = "Column offset K2 column:  " + QString::number(IJK_accuracy_list[1][3]);
//            str_out.append("\n" + temp_str);
            std::cout << str_out.toStdString() << std::endl;
            Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

            // Cosine measure
            str_out = "Column IJK cosine measure: angle b/e vectors (1=parallel=best)";
            std::cout << "\033[1;33m" << str_out.toStdString() << "\033[0m" << std::endl;
            Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

            str_out  = "Column cosine I column: " + QString::number(IJK_accuracy_list[2][0]);
            temp_str = "Column cosine J column: " + QString::number(IJK_accuracy_list[2][1]);
            str_out.append("\n" + temp_str);
            temp_str = "Column cosine K column: " + QString::number(IJK_accuracy_list[2][2]);
            str_out.append("\n" + temp_str);
//            temp_str = "Column cosine K2 column: " + QString::number(IJK_accuracy_list[2][3]);
//            str_out.append("\n" + temp_str);
            std::cout << str_out.toStdString() << std::endl;
            Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

			return IJK_accuracy_list;
        }

/*!
 * \brief
 * \param
 * \return
 */
        template<typename T, typename V> void CheckRowwiseDiff(T& va_, T& vb_, V& vdiff_, QString tag, double tol, WIData data){

            // Check vector has length > 0
            if (!vdiff_.rows()>0)
                throw std::runtime_error("Difference vector (vdiff_) has no elements");

            auto vrel_ = va_.cwiseQuotient(vb_);

            // Output msg
            QString str_out;
            str_out = "Testing: " + tag + " (rowwise) >> values differ at the following rows:";
            std::cout << "\033[1;33m" << str_out.toStdString() << "\033[0m" << std::endl;
            Utilities::FileHandling::WriteLineToFile(str_out, data.tex_file);

            // Loop over each row
            for( int ii=0; ii < vdiff_.rows(); ++ii ) {

                // Setup row
                auto vdiff_row = vdiff_.row(ii);

                // print out to double vectors for QString treatment later on
                std::vector<double> va_d, vb_d, vdiff_d, vrel_d;
                QStringList labels;
                labels << "RMS: " << "PCG: " << "DFF: " << "RMS/PCG: ";
                string frmt;
                int ncols;

                for( int jj = 0; jj < vdiff_row.size(); ++jj ) {
                    va_d.push_back(va_(ii,jj));
                    vb_d.push_back(vb_(ii,jj));
                    vdiff_d.push_back(vdiff_(ii,jj));
                    vrel_d.push_back(vrel_(ii,jj));
                }

                if (tag.compare("IJK")==0){
                    frmt = "%2.0f ";
                    ncols = va_d.size() - 1; // SKIP LAST COLUMN
                }else if(tag.compare("WCF")==0){
                    frmt = "%7.3f    ";
                    ncols = va_d.size();
                }

                // If difference is larger than zero by a given tolerance
                if (!vdiff_row.isZero(tol)){

                    QString num_str, txt_str;
                    num_str.sprintf("row %3.0f:  ", (double) ii);
                    txt_str.append(num_str);

                    txt_str.append(labels[0]); // RMS DATA
                    for( int jj = 0; jj < ncols; ++jj ) {
                        txt_str.append(num_str.sprintf(frmt.c_str(), va_d[jj]));
                    }

                    txt_str.append(labels[1]); // PCG DATA
                    for( int jj = 0; jj < ncols; ++jj ) {
                        txt_str.append(num_str.sprintf(frmt.c_str(), vb_d[jj]));
                    }

                    txt_str.append(labels[2]); // DFF DATA
                    for( int jj = 0; jj < ncols; ++jj ) {
                        txt_str.append(num_str.sprintf(frmt.c_str(), vdiff_d[jj]));
                    }

                    txt_str.append(labels[3]); // DFF DATA
                    for( int jj = 0; jj < ncols; ++jj ) {
                        txt_str.append(num_str.sprintf(frmt.c_str(), vrel_d[jj]));
                    }

                    std::cout << txt_str.toStdString() << std::endl;
                    Utilities::FileHandling::WriteLineToFile(txt_str, data.tex_file);
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
            QString tag = "IJK";
			CheckRowwiseDiff(va_, vb_, vdiff_, tag, GetEpsIJK(), va);
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
            QString tag = "WCF";
			CheckRowwiseDiff(va_, vb_, vdiff_, tag, GetEpsWCF(), va);
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
            QString str_out;
            QString lstr_out = "\n--------------------------------------------------------------------------------";
            QString tol;
            tol.sprintf("%5.3e",GetEpsIJK());

            if (vdiff.IJK.isZero(GetEpsIJK())){
                str_out = lstr_out + "\nIJK values match exactly for this well.";
                std::cout << "\033[1;32m" << str_out.toStdString() << "\033[0m" << std::endl;
                Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

            }else{
                str_out = lstr_out + "\nIJK values are NOT the same for this well.";
                std::cout << "\033[1;35m" << str_out.toStdString() << "\033[0m" << std::endl;
                Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

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

            QString str_out;
            QString lstr_out = "\n--------------------------------------------------------------------------------";
            QString tol;
            tol.sprintf("%5.3f",GetEpsWCF());

            if(va.WCF.isApprox(vb.WCF, GetEpsWCF())){
                str_out = lstr_out + "\nWCF values match exactly for this well (WCF tol = " + tol + ").";
                std::cout << "\033[1;32m" << str_out.toStdString() << "\033[0m" << std::endl;
                Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);
            }else{
                str_out = lstr_out + "\nWCF values are NOT the same for this well (WCF tol = " + tol + ").";
                std::cout << "\033[1;35m" << str_out.toStdString() << "\033[0m" << std::endl;
                Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

                // Output general difference (i.e., for I, J and K columns)
                CheckRowwiseDiffWCF(va,vb,vdiff);

                // Output difference for column
                WCF_accuracy_list.append(GetColumnAccuracyElements(vdiff.WCF));
                WCF_accuracy_list.append(GetColumnOffset(va.WCF, vb.WCF, vdiff.WCF));
                WCF_accuracy_list.append(GetColumnCosine(va.WCF, vb.WCF, vdiff.WCF));

                // Zero element fraction
                str_out = "\nElement accuracy: fraction of zero (<tol) elements in diff. column (1=best)";
                std::cout << "\033[1;33m" << str_out.toStdString() << "\033[0m" << std::endl;
                Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

                str_out  = "Element accuracy:  " + QString::number(WCF_accuracy_list[0]);
                std::cout << str_out.toStdString() << std::endl;
                Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

                // Column offset
                str_out = "Column WCF offset: norm of difference vector (0=best)";
                std::cout << "\033[1;33m" << str_out.toStdString() << "\033[0m" << std::endl;
                Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

                str_out  = "Column offset:  " + QString::number(WCF_accuracy_list[1]);
                std::cout << str_out.toStdString() << std::endl;
                Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

                // Cosine measure
                str_out = "Column WCF cosine measure: angle b/e vectors (1=parallel=best)";
                std::cout << "\033[1;33m" << str_out.toStdString() << "\033[0m" << std::endl;
                Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);

                str_out  = "Column cosine measure:  " + QString::number(WCF_accuracy_list[2]);
                std::cout << str_out.toStdString() << std::endl;
                Utilities::FileHandling::WriteLineToFile(str_out, va.tex_file);
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
            if (debug_){
                std::cout << "WILong.IJK.rows:" << WILong.IJK.rows() << std::endl;
                std::cout << "WIShort.IJK.rows:" << WIShort.IJK.rows() << std::endl;
            }


            // LOOP OVER ALL ROWS IN THE LONGEST COLUMN AND INSERT EACH OF THESE INTO A NEW IJK
            // COLUMN UNLESS THE GIVEN ROW IS A SUPERFLUOUS ONE, IN WHICH CASE WE SKIP IT
            int kk = 0;
            for (int ii = 0; ii < WILong.IJK.rows(); ++ii) {
                if (debug_) std::cout << "ii: " <<  ii
                                      << "[kk: " << kk << "] "
                                      << "is current row superfluous? (1=yes, 0=no): " << sup_indices.contains(ii)
                                      << std::endl;
                if (! sup_indices.contains(ii)) {
                    WIShort.IJK.row(kk) << WILong.IJK.row(ii);
                    WIShort.WCF.row(kk) << WILong.WCF.row(ii);
                    kk += 1;
                }else{
                    if (debug_) std::cout << "Row not added to short vector!" << std::endl;;
                }
            }

            // MAKE ORIGINALLY (TOO-)LONG COLUMN EQUAL TO COLUMN WITHOUT SUPERFLUOUS ROWS
            QString rem_str;
            if (WIDataRMS.IJK.rows() > WIDataPCG.IJK.rows()) {
                WIDataRMS.IJK = WIShort.IJK;
                WIDataRMS.WCF = WIShort.WCF;
                rem_str = "RMS";
            } else {
                WIDataPCG.IJK = WIShort.IJK;
                WIDataPCG.WCF = WIShort.WCF;
                rem_str = "PCG";
            }

            if (debug_){
                std::cout << "WIDataRMS.IJK.rows:" << WIDataRMS.IJK.rows() << std::endl;
                std::cout << "WIDataPCG.IJK.rows:" << WIDataPCG.IJK.rows() << std::endl;
                std::cout << "sup_indices.size():" << sup_indices.size() << std::endl;
            }

            // VECTOR LENGTHS HAVE BEEN MADE EQUAL => COMPARE DIRECTLY
            QString str_out;
            QString ind_str = (sup_indices.length() > 1) ? QString::number(sup_indices.size()) + " rows were" : "1 row was";
            str_out.append(WIDataRMS.dir_name + ">>> Vector lengths have been made equal: "
                                              + ind_str + " removed from "
                                              + rem_str + "\ndata b/c IJK values did not match. ");

            QStringList str_ind;
            foreach(int ii, sup_indices){ str_ind.append(QString::number(ii)); }
            str_out.append("Rows that were removed: [" + str_ind.join(" ") + "].");


            if (sup_indices.length()>10){
                str_out.append("\nWARNING: more than 10 rows were removed, check wells are supposed to be equal.");
            }

            str_out.append("\nContinuing comparison.");
            std::cout << "\033[1;36m" << str_out.toStdString() << "\033[0m" << std::endl;
            Utilities::FileHandling::WriteLineToFile(str_out, WIDataPCG.tex_file);
        }

/*!
 * \brief
 * \param
 * \return
 */
        void RemoveSuperfluousRowsWrapper(WIData &WIDataRMS,
                                          WIData &WIDataPCG,
                                          QStringList &dir_list_,
                                          QStringList &dir_names_,
                                          int ii){

            // PRINT IJK, WCF DATA TO INDIVIDUAL FILES TO TREAT WITH diff COMMAND LATER:
            // MAKE DIFF FILE NAMES
            QStringList diff_files = {
                    dir_list_[ii] + "/DIFF_" + dir_names_[ii] + "_RMS.IJK",
                    dir_list_[ii] + "/DIFF_" + dir_names_[ii] + "_PCG.IJK",
                    dir_list_[ii] + "/DIFF_" + dir_names_[ii] + "_RMS.WCF",
                    dir_list_[ii] + "/DIFF_" + dir_names_[ii] + "_PCG.WCF"
            };

            // PRINT DIFF FILES
            WIDataRMS.PrintIJKData(diff_files[0]);
            WIDataPCG.PrintIJKData(diff_files[1]);
            WIDataRMS.PrintWCFData(diff_files[2]);
            WIDataPCG.PrintWCFData(diff_files[3]);

            QString str_out;
            QString lstr_out = "\n--------------------------------------------------------------------------------";
            if (DiffVectorLength(WIDataRMS, WIDataPCG)) {

                // IF VECTOR LENGTHS ARE EQUAL => COMPARE DIRECTLY
                str_out = lstr_out + "\n" + WIDataRMS.dir_name 
                + ">>> COMPDAT vectors have the same length. Making comparison.";
                std::cout << std::endl << "\033[1;36m" << str_out.toStdString() << "\033[0m" << std::endl;
                Utilities::FileHandling::WriteLineToFile(str_out, WIDataPCG.tex_file);

            } else {

                // IF VECTOR LENGTHS ARE UNEQUAL => MAKE EQUAL, THEN COMPARE DIRECTLY
                str_out = lstr_out + "\n" + WIDataRMS.dir_name 
                + ">>> COMPDAT vectors have different length. Making them equal.";
                std::cout << std::endl << "\033[1;36m" << str_out.toStdString() << "\033[0m" << std::endl;
                Utilities::FileHandling::WriteLineToFile(str_out, WIDataPCG.tex_file);

                RemoveSuperfluousRows(WIDataRMS, WIDataPCG, diff_files);
            }
        }

        // ///////////////////////////////////////////////////////////////////////////////
        // DEBUG MESSAGES
        template<typename ZA, typename ZB, typename ZC, typename ZD, typename ZE, typename ZF>
        void debug_msg(bool debug_, string msg,
                       ZA varA, // dir_names_
                       ZB varB, // dir_list_
                       ZC varC, // ii
                       ZD varD, // WIDataRMS
                       ZE varE, // WIDataPCG
                       ZF varF  // empty
        ){

            if (msg.compare("well_dir_list")==0){
                if (debug_) {
                    std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl;
                    for (int ii = 0; ii < varA.length(); ++ii) {
                        std::cout << ii << ":" << varA[ii].toStdString() << std::endl; // list of well dirs (names only)
                        std::cout << ii << ":" << varB[ii].toStdString() << std::endl; // list of well dirs (absolute path)
                    }
                    std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
                }
            }else if(msg.compare("RMS_PCG_IJK_data")==0){
                if (debug_) {
                    int nRMS = (varD.IJK.rows() > 5) ? 5 : varD.IJK.rows();
                    int nPCG = (varE.IJK.rows() > 5) ? 5 : varE.IJK.rows();

                    std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl << std::setfill(' ');

                    // RMS-PCG: IJK
                    std::cout << "RMS-PCG IJK DATA (well = " << varA[varC].toStdString() << ")" << std::endl;
                    // RMS: IJK
                    std::cout << "WIDataRMS.IJK (size: " << varD.IJK.size() << "): "
                              << std::endl << varD.IJK.block(0, 0, nRMS, 4)
                              << std::endl << "..." << std::endl;
                    // PCG: IJK
                    std::cout << "WIDataPCG.IJK (size: " << varE.IJK.size() << "): "
                              << std::endl << varE.IJK.block(0, 0, nPCG, 4)
                              << std::endl << "..." << std::endl;

                    // RMS-PCG: WCF
                    std::cout << "RMS-PCG WCF DATA (well = " << varA[varC].toStdString() << ")" << std::endl;
                    // RMS: WCF
                    std::cout << "WIDataRMS.WCF: (size: " << varD.WCF.size() << "): "
                              << std::endl << varD.WCF.block(0, 0, nRMS, 1)
                              << std::endl << "..." << std::endl;
                    // PCG: WCF
                    std::cout << "WIDataPCG.WCF: (size: " << varE.WCF.size() << "): "
                              << std::endl << varE.WCF.block(0, 0, nPCG, 1)
                              << std::endl << "..." << std::endl;

                    std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
                    // std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
                }
            }else if(msg.compare("C")==0){

            }else if(msg.compare("D")==0){

            }else if(msg.compare("E")==0){

            }

        }

    }
}
#endif //FIELDOPT_TEST_RESOURCE_DIFF_FUNCTIONS_H
