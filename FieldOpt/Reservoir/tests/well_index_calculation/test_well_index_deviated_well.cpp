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
#include "Reservoir/tests/test_resource_wic_widata.h"

#include <QList>
#include <QVector>
#include <QTextStream>
#include <fstream>
#include <Eigen/Dense>

using namespace Reservoir::Grid;
using namespace Reservoir::WellIndexCalculation;
using namespace TestResources;

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
    };

    TEST_F(DeviatedWellIndexTest, test) {

        //        debug
        //        for( int ii=0; ii < in_fields.length(); ++ii ) {
        //            std::cout << ii << ":" << in_fields[ii].toStdString() << std::endl;
        //        }

    }

    TEST_F(DeviatedWellIndexTest, compareCOMPDAT) {

        std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
        std::cout << "" << std::endl;

        // FIND LIST OF WELL FOLDERS CONTAINING PCG & RMS COMPDATS
        auto file_list_ = well_dir_->GetWellDir();
        auto rms_files = file_list_[1];
        auto pcg_files = file_list_[2];

        // LOOP THROUGH LIST OF WELL FOLDERS: FOR WELL FOLDER ii,
        // READ PCG & RMS COMPDAT DATA
//        for( int ii=0; ii < rms_files.length(); ++ii ){

            WIData WIDataRMS;
            WIDataRMS.ReadData(rms_files[0]);

            WIData WIDataPCG;
            WIDataPCG.ReadData(pcg_files[0]);

        auto va = WIDataRMS.IJK;
        auto vb = WIDataPCG.IJK;

        if (va.rows() != vb.rows()){

        }else{

        }
//        }



// debug:
//        std::cout << "WIDataPCG DATA " << std::endl;
//        std::cout << "WIDataPCG.IJK " << WIDataPCG.IJK << std::endl;
//        std::cout << "WIDataPCG.WIC " << WIDataPCG.WCF << std::endl;

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