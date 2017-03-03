/******************************************************************************
   Copyright (C) 2016-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#ifndef FIELDOPT_TEST_RESOURCE_EXAMPLE_FILE_PATHS_H
#define FIELDOPT_TEST_RESOURCE_EXAMPLE_FILE_PATHS_H

#include <QString>

namespace TestResources {
    namespace ExampleFilePaths {
        static QString driver_example_ = "../examples/driver.json";
        static QString directory_output_ = "../fieldopt_output";

        static QString grid_horzwel_ = "../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID";
        static QString grid_5spot_ = "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
//        static QString grid_5spot_flow_ = "../examples/Flow/5spot/5SPOT.EGRID";

        static QString ecl_base_horzwell = "../examples/ECLIPSE/HORZWELL/HORZWELL";

        static QString driver_5spot_ = "../examples/ADGPRS/5spot/fo_driver_5vert_wells.json";
        static QString gprs_drv_5spot_ = "../examples/ADGPRS/5spot/5SPOT.gprs";
        static QString gprs_smry_json_5spot_ = "../examples/ADGPRS/5spot/5SPOT.json";
        static QString gprs_smry_hdf5_5spot_ = "../examples/ADGPRS/5spot/5SPOT.vars.h5";
        static QString gprs_base_5spot_ = "../examples/ADGPRS/5spot/5SPOT";

//        static QString driver_5spot_flow_ = "../examples/Flow/5spot/fo_driver_2_horz_placement.json";
//        static QString flow_drv_5spot_ = "../examples/Flow/5spot/5SPOT.DATA";
//        static QString flow_base_5spot_ = "../exampels/Flow/5spot";

    }
}

#endif //FIELDOPT_TEST_RESOURCE_EXAMPLE_FILE_PATHS_H
