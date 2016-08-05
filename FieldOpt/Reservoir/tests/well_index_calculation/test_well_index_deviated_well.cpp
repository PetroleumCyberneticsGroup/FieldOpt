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
        bool debug_ = true;
        int num_files = 1;
    };

    TEST_F(DeviatedWellIndexTest, test) {

    }

    TEST_F(DeviatedWellIndexTest, compareCOMPDAT) {

        // FIND LIST OF WELL FOLDERS CONTAINING PCG & RMS COMPDATS
        auto file_list_ = well_dir_->GetWellDir();
        auto dir_list_  = file_list_[0]; // list of well dirs (absolute path)
        auto dir_names_ = file_list_[1]; // list of well dirs (names only)
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
        if (!debug_) num_files=rms_files.length();
        for( int ii=0; ii < num_files; ++ii ){

            WIDataRMS.ReadData(rms_files[ii]);
            WIDataPCG.ReadData(pcg_files[ii]);

            if (debug_){
                std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl;
                std::cout << "WIDataPCG DATA " << std::endl;
                std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
                std::cout << "WIDataPCG.IJK (size: " << WIDataPCG.IJK.size() << "): "
                          << std::endl << WIDataPCG.IJK.block(0,0,10,4)
                          << std::endl << "..." << std::endl;
                std::cout << "WIDataPCG.WIC: (size: " << WIDataPCG.WCF.size() << "): "
                          << std::endl << WIDataPCG.WCF.block(0,0,10,1)
                          << std::endl << "..." << std::endl;
                std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
            }

            if(DiffVectorLength(WIDataRMS,WIDataPCG)){

                std::cout << "Checking IJK and WCF data for well: "
                          << WIDataRMS.dir_name.toStdString() << std::endl;
                std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
                
                CompareIJK(WIDataRMS,WIDataPCG);
                CompareWCF(WIDataRMS,WIDataPCG);

            }else{

            }

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