//
// Created by bellout on 7/30/16.
//

#include <gtest/gtest.h>
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include "Utilities/file_handling/filehandling.h"
#include "Reservoir/well_index_calculation/wellindexcalculator.h"

#include "Reservoir/tests/test_resource_wic_welldir.h"
#include "Reservoir/tests/test_resource_wic_diff_functions.h"

#include <QList>
#include <QVector>
#include <QProcess>
#include <QTextStream>
#include <fstream>
#include <Eigen/Dense>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QTextCodec>

using namespace Reservoir::Grid;
using namespace Reservoir::WellIndexCalculation;
using namespace TestResources::WIBenchmark;

namespace {

    class DeviatedWellIndexTest : public ::testing::Test {

    protected:
        DeviatedWellIndexTest() {
            grid_ = new ECLGrid(file_path_);
            well_dir_ = new WellDir();
        }

        virtual ~DeviatedWellIndexTest() {
            delete grid_;
        }

        Grid *grid_;
        WellDir *well_dir_;
        QString file_path_ = "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
        bool debug_ = false;
    };

    TEST_F(DeviatedWellIndexTest, test) {

    }

    TEST_F(DeviatedWellIndexTest, compareCOMPDAT) {

        // FIND LIST OF WELL FOLDERS CONTAINING PCG & RMS COMPDATS
        auto file_list_ = well_dir_->GetWellDir();
        auto dir_names_ = file_list_[0]; // list of well dirs (names only) => dir name only: tw04_04
        auto dir_list_ = file_list_[1]; // list of well dirs (absolute path) => fullpath: ../tw04_04/
        auto rms_files = file_list_[2]; // fullpath: ../tw04_04/EVENTS_tw04_04_RMS.DATA
        auto pcg_files = file_list_[3]; // fullpath: ../tw04_04/EVENTS_tw04_04_PCG.DATA

        if (debug_) {
            std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl;
            for (int ii = 0; ii < dir_names_.length(); ++ii) {
                std::cout << ii << ":" << dir_names_[ii].toStdString() << std::endl; // list of well dirs (names only)
                std::cout << ii << ":" << dir_list_[ii].toStdString() << std::endl; // list of well dirs (absolute path)
            }
            std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
        }

        // WELL INDEX DATA OBJECTS
        WIData WIDataRMS, WIDataPCG;

        // LOOP THROUGH LIST OF WELL FOLDERS: FOR WELL FOLDER ii,
        // READ PCG & RMS COMPDAT DATA
        int num_files = (debug_) ? 1 : rms_files.length();

        for (int ii = 0; ii < num_files; ++ii) {

            // READ COMPDAT FILES
            WIDataRMS.ReadData(rms_files[ii]);
            WIDataPCG.ReadData(pcg_files[ii]);

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

            // MESSAGE OUTPUT
            std::cout << "\n\n\033[1;36m" << std::setfill('=') << std::setw(80) << "=" << "\033[0m" << std::endl;
            std::cout << "\033[1;36mChecking IJK and WCF data for well: "
                      << WIDataRMS.dir_name.toStdString() << "\033[0m" << std::endl;
            std::cout << "\033[1;36m" << std::setfill('=') << std::setw(80) << "=" << "\033[0m" << std::endl;

            if (debug_) {
                int nRMS = (WIDataRMS.IJK.rows() > 5) ? 5 : WIDataRMS.IJK.rows();
                int nPCG = (WIDataPCG.IJK.rows() > 5) ? 5 : WIDataPCG.IJK.rows();
                // std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
                std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl;

                // RMS-PCG: IJK
                std::cout << "RMS-PCG IJK DATA (well = " << dir_names_[ii].toStdString() << ")" << std::endl;
                // RMS: IJK
                std::cout << "WIDataRMS.IJK (size: " << WIDataRMS.IJK.size() << "): "
                          << std::endl << WIDataRMS.IJK.block(0, 0, nRMS, 4)
                          << std::endl << "..." << std::endl;
                // PCG: IJK
                std::cout << "WIDataPCG.IJK (size: " << WIDataPCG.IJK.size() << "): "
                          << std::endl << WIDataPCG.IJK.block(0, 0, nPCG, 4)
                          << std::endl << "..." << std::endl;

                // RMS-PCG: WCF
                std::cout << "RMS-PCG WCF DATA (well = " << dir_names_[ii].toStdString() << ")" << std::endl;
                // RMS: WCF
                std::cout << "WIDataRMS.WCF: (size: " << WIDataRMS.WCF.size() << "): "
                          << std::endl << WIDataRMS.WCF.block(0, 0, nRMS, 1)
                          << std::endl << "..." << std::endl;
                // PCG: WCF
                std::cout << "WIDataPCG.WCF: (size: " << WIDataPCG.WCF.size() << "): "
                          << std::endl << WIDataPCG.WCF.block(0, 0, nPCG, 1)
                          << std::endl << "..." << std::endl;

                std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
                // std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
            }

            if (DiffVectorLength(WIDataRMS, WIDataPCG)) {

                // IF VECTOR LENGTHS ARE EQUAL => COMPARE DIRECTLY
                std::cout << "\033[1;36m" << WIDataRMS.dir_name.toStdString() <<
                          ": >>> Vector lengths are equal. Making comparison.\033[0m" <<
                          std::setfill(' ') << std::endl;

                CompareIJK(WIDataRMS, WIDataPCG);
                CompareWCF(WIDataRMS, WIDataPCG);

            } else {

                // IF VECTOR LENGTHS ARE UNEQUAL => MAKE EQUAL, THEN COMPARE DIRECTLY
                std::cout << "\033[1;36m" << WIDataRMS.dir_name.toStdString() <<
                          ": >>> Vector lengths are unequal. Making them equal.\033[0m" << std::endl;

                // RemoveSuperfluousRows(WIDataRMS,WIDataPCG,diff_files)

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
                    }
                    kk += 1;
                }

                // MAKE ORIGINALLY (TOO-)LONG COLUMN EQUAL TO COLUMN WITHOUT SUPERFLUOUS ROWS
                if (WIDataRMS.IJK.rows() > WIDataPCG.IJK.rows()) {
                    WIDataRMS.IJK = WIShort.IJK;
                } else {
                    WIDataPCG.IJK = WIShort.IJK;
                }

                if (debug_){
                    std::cout << "WIDataRMS.IJK.rows:" << WIDataRMS.IJK.rows() << std::endl;
                    std::cout << "WIDataPCG.IJK.rows:" << WIDataPCG.IJK.rows() << std::endl;
                }

                // VECTOR LENGTHS HAVE BEE MADE EQUAL => COMPARE DIRECTLY
                std::cout << "\033[1;36m" << WIDataRMS.dir_name.toStdString() <<
                          ": >>> Vector lengths have been made equal. Making comparison.\033[0m" <<
                          std::setfill(' ') << std::endl;

                CompareIJK(WIDataRMS, WIDataPCG);
                CompareWCF(WIDataRMS, WIDataPCG);
            }
        }
    }
}