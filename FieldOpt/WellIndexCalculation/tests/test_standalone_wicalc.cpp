/******************************************************************************
   Copyright (C) 2017 Mathias C. Bellout <mathias.bellout@ntnu.no>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>
   Modified by Einar Baumann (2017) <einar.baumann@gmail.com>

   This file and the WellIndexCalculator as a whole is part of the
   FieldOpt project. However, unlike the rest of FieldOpt, the
   WellIndexCalculator is provided under the GNU Lesser General Public
   License.

   WellIndexCalculator is free software: you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation, either version 3 of
   the License, or (at your option) any later version.

   WellIndexCalculator is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with WellIndexCalculator.  If not, see
   <http://www.gnu.org/licenses/>.
******************************************************************************/

/*!
 * \todo Add more tests to check the error handing capability, e.g.,
 * against wrong/missing parameter input, if sufficient. If not,
 * increase the capability (in main.hpp).
 */

#include <gtest/gtest.h>
#include <boost/filesystem.hpp> /* exists */
#include <stdio.h>              /* printf */
#include <stdlib.h>             /* system, NULL, EXIT_FAILURE */

using namespace std;

namespace{

class wicalcStandaloneTest : public ::testing::Test {
 protected:
  wicalcStandaloneTest(){};

  virtual ~wicalcStandaloneTest(){}

  virtual void SetUp(){}

  virtual void TearDown(){}

  string wicalc_path = "./wicalc";
  string grid_str =    " --grid ../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
  string heel_str =    " --heel 10 10 1712";
  string toe_str =     " --toe 100 100 1712";
  string radius_str =  " --radius 0.1905";
  string skin_str =    " --skin-factor 0.0";
  string compdat_str = " --compdat";
  string wname_str =   " --well-name PROD";

  bool check_wicalc_exists(){
      if ( !boost::filesystem::exists( wicalc_path ) ){
          printf ("Cannot find wicalc executable. Skipping test.\n");
          return false;
      }else{
          return true;
      }
  };
};

TEST_F(wicalcStandaloneTest, checkExecutable) {
    if (check_wicalc_exists()){
        string cmd_in = wicalc_path +
            grid_str +
            heel_str +
            toe_str +
            radius_str +
            skin_str +
            compdat_str +
            wname_str;

        printf ("Executing: %s.\n", cmd_in.c_str());
        // \todo This does not work on Windows native or with Cygwin
        int i=system(cmd_in.c_str());
        EXPECT_EQ(i, 0);

        // \todo Pipe the output of system to a string variable
        // then compare against a similar output
    }
}

TEST_F(wicalcStandaloneTest, checkMissingInputParameterGrid) {
    if (check_wicalc_exists()){

        string cmd_in = wicalc_path +
            " " +
            heel_str +
            toe_str +
            radius_str +
            skin_str +
            compdat_str +
            wname_str;

        printf ("Executing: %s.\n", cmd_in.c_str());
        // \todo This does not work on Windows native or with Cygwin
        int i=system(cmd_in.c_str());
        EXPECT_EQ(i, 0);
    }
}

// TEST_F(wicalcStandaloneTest, checkErrorHandingInputParameterGrid) {
// string grid_str_err = " --grid../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
// }

}
