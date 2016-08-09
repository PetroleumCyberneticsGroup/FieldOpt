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
#include <QTextStream>
#include <fstream>
#include <Eigen/Dense>

#include "../../../external_libraries/diff_match_patch/diff_match_patch_20121119/cpp/diff_match_patch.cpp"

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
        auto dir_names_ = file_list_[0]; // list of well dirs (names only)
        auto dir_list_  = file_list_[1]; // list of well dirs (absolute path)
        auto rms_files  = file_list_[2];
        auto pcg_files  = file_list_[3];

        if (debug_) {
            std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl;
            for (int ii = 0; ii < dir_names_.length(); ++ii) {
                std::cout << ii << ":" << dir_names_[ii].toStdString() << std::endl; // list of well dirs (names only)
                std::cout << ii << ":" << dir_list_[ii].toStdString() << std::endl; // list of well dirs (absolute path)
            }
            std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
        }


        WIData WIDataRMS, WIDataPCG;

        // LOOP THROUGH LIST OF WELL FOLDERS: FOR WELL FOLDER ii,
        // READ PCG & RMS COMPDAT DATA
        int num_files = (debug_) ? 1 : rms_files.length();

        for( int ii=0; ii < num_files; ++ii ){

            WIDataRMS.ReadData(rms_files[ii]);
            WIDataPCG.ReadData(pcg_files[ii]);

            std::cout << "\n\n\033[1;36m" << std::setfill('=') << std::setw(80) << "=" << "\033[0m" << std::endl;
            std::cout << "\033[1;36mChecking IJK and WCF data for well: "
                      << WIDataRMS.dir_name.toStdString() << "\033[0m" << std::endl;
            std::cout << "\033[1;36m" << std::setfill('=') << std::setw(80) << "=" << "\033[0m" << std::endl;

            if (debug_){
                int nRMS = (WIDataRMS.IJK.rows() > 5) ? 5 : WIDataRMS.IJK.rows();
                int nPCG = (WIDataPCG.IJK.rows() > 5) ? 5 : WIDataPCG.IJK.rows();
                // std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
                std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl;

                // RMS-PCG: IJK
                std::cout << "RMS-PCG IJK DATA (well = " << dir_names_[ii].toStdString() << ")" << std::endl;
                // RMS: IJK
                std::cout << "WIDataRMS.IJK (size: " << WIDataRMS.IJK.size() << "): "
                          << std::endl << WIDataRMS.IJK.block(0,0,nRMS,4)
                          << std::endl << "..." << std::endl;
                // PCG: IJK
                std::cout << "WIDataPCG.IJK (size: " << WIDataPCG.IJK.size() << "): "
                          << std::endl << WIDataPCG.IJK.block(0,0,nPCG,4)
                          << std::endl << "..." << std::endl;

                // RMS-PCG: WCF
                std::cout << "RMS-PCG WCF DATA (well = " << dir_names_[ii].toStdString() << ")" << std::endl;
                // RMS: WCF
                std::cout << "WIDataRMS.WCF: (size: " << WIDataRMS.WCF.size() << "): "
                          << std::endl << WIDataRMS.WCF.block(0,0,nRMS,1)
                          << std::endl << "..." << std::endl;
                // PCG: WCF
                std::cout << "WIDataPCG.WCF: (size: " << WIDataPCG.WCF.size() << "): "
                          << std::endl << WIDataPCG.WCF.block(0,0,nPCG,1)
                          << std::endl << "..." << std::endl;
                
                std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
                // std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
            }

            if(DiffVectorLength(WIDataRMS,WIDataPCG)) {

                // If vector lengths are equal => compare directly
                std::cout << "\033[1;36m" << WIDataRMS.dir_name.toStdString() <<
                ": >>> Vector lengths are equal. Making comparison.\033[0m" << 
                std::setfill(' ') << std::endl;

                CompareIJK(WIDataRMS, WIDataPCG);
                CompareWCF(WIDataRMS, WIDataPCG);

            }else {

                // If vector lengths are unequal => make equal, then compare directly
                std::cout << "\033[1;36m" << WIDataRMS.dir_name.toStdString() <<
                ": >>> Vector lengths are unequal. Making them equal.\033[0m" << std::endl;

                // diff_match_patch* dmp; //
                auto dmp = new diff_match_patch();
                QString str1 = "abc"; 
                QString str2 = "abd";
                // QList<Diff> diffs;

                QList<Diff> diffs = dmp->diff_main(str1,str2);

                // QList<QVariant> a = dmp->diff_linesToChars("abc", "xyz");
                // QVariant lineText1 = a[0];
                // QVariant lineText2 = a[1];
                // QVariant lineArray = a[2];                

                // QList<Diff> diffs = dmp->diff_main(lineText1, lineText2, false);

                // dmp->diff_charsToLines_(diffs, lineArray);
                // std::cout << diffs[0];
                std::cout << "Diffs: " << diffs[0].toString().toStdString();
            }


        DiffVectorLength(WIDataRMS,WIDataPCG);

//        if(TestResources::WIBenchmark::DiffVectorLength(WIDataRMS,WIDataPCG)){
//            TestResources::WIBenchmark::CompareIJK(WIDataRMS,WIDataPCG);
//        }
//
//        TestResources::WIBenchmark::CompareIJK(WIDataRMS,WIDataPCG);

        }



// debug:
//        std::cout << "WIDataPCG DATA " << std::endl;
//        std::cout << "WIDataPCG.IJK " << WIDataPCG.IJK << std::endl;
//        std::cout << "WIDataPCG.WIC " << WIDataPCG.WCF << std::endl;

//        for( int ii=0; ii < in_fields.length(); ++ii ) {
//            std::cout << ii << ":" << in_fields[ii].toStdString() << std::endl;
//        }

//        COMPARE COMPDAT DATA
//
//        1. COMPARE NUMBER OF ROWS (NROWS)
//        	1.1	IF (NROWS) NOT EQUAL
//        	THEN ...
//        	ELSE CONTINUE TO STEP 2
//
//        2. COMPARE IJK COORDS
//	        2.1 IF (IJK) NOT EQUAL
//	        THEN: DETERMINE DEGREE OF DIFFERENCE
//	        ELSE: DO NOTHING, CONTINUE TO STEP 3
//
//        3. COMPARE WI VALUES



    }
}