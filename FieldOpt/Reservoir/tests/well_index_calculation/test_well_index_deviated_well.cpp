/******************************************************************************
   Copyright (C) 2016-2017 Mathias C. Bellout <mathias.bellout@ntnu.no>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


#include <gtest/gtest.h>
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include "Utilities/file_handling/filehandling.h"
#include "Reservoir/well_index_calculation/wellindexcalculator.h"
#include "Reservoir/tests/test_resource_wic.h"

#include <QList>
#include <QVector>
#include <QTextStream>
#include <fstream>
#include <iostream>
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


TEST_F(DeviatedWellIndexTest, test){}


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

//if (va.rows() != vb.rows()){
//
//}else{
//
//}

// debug:
//        std::cout << "WIDataPCG DATA " << std::endl;
//        std::cout << "WIDataPCG.IJK " << WIDataPCG.IJK << std::endl;
//        std::cout << "WIDataPCG.WIC " << WIDataPCG.WCF << std::endl;

// Keep to finish test
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

} // namespace
