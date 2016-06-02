//
// Created by einar on 6/2/16.
//

#ifndef FIELDOPT_TEST_RESOURCE_EXAMPLE_FILE_PATHS_H
#define FIELDOPT_TEST_RESOURCE_EXAMPLE_FILE_PATHS_H

#include <QString>

namespace TestResources {
    namespace ExampleFilePaths {
        static QString driver_example_ = "../examples/driver.json";
        static QString directory_output_ = "../fieldopt_output";

        static QString grid_horzwel_ = "../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID";
        static QString grid_5spot_ = "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";

        static QString ecl_drv_horzwell = "../examples/ECLIPSE/HORZWELL/HORZWELL";

        static QString driver_5spot_ = "../examples/ADGPRS/5spot/5spot_fieldopt_driver.json";
        static QString gprs_drv_5spot_ = "../examples/ADGPRS/5spot/5SPOT.gprs";
        static QString gprs_smry_json_5spot_ = "../examples/ADGPRS/5spot/5SPOT.json";
        static QString gprs_smry_hdf5_5spot_ = "../examples/ADGPRS/5spot/5SPOT.SIM.H5";
        static QString gprs_base_5spot_ = "../examples/ADGPRS/5spot/5SPOT";
    }
}

#endif //FIELDOPT_TEST_RESOURCE_EXAMPLE_FILE_PATHS_H
