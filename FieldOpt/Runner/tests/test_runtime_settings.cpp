#include <gtest/gtest.h>
#include "Runner/runtime_settings.h"

namespace {

    class RuntimeSettingsTest : public testing::Test {
        int argc = 16;
        const char *argv[16] = {"FieldOpt",
                        "/home/einar/Documents/GitHub/PCG/FieldOpt/examples/ADGPRS/5spot/fo_driver_5vert_wells.json",
                        "/home/einar/fieldopt_output",
                        "-g", "/home/einar/Documents/GitHub/PCG/FieldOpt/examples/Flow/5spot/5SPOT.EGRID",
                        "-s", "/home/einar/Documents/GitHub/PCG/FieldOpt/examples/Flow/5spot/5SPOT.DATA",
                        "-b", "/home/einar/.CLion2016.2/system/cmake/generated/FieldOpt-c9373114/c9373114/Debug/bin",
                        "-r", "mpisync",
                        "-f",
                        "-v", "2",
                        "-t", "1000"
        };


    protected:

        RuntimeSettingsTest() {
            rts = new Runner::RuntimeSettings(argc, argv);
        }
        ~RuntimeSettingsTest(){}
        Runner::RuntimeSettings *rts;
    };

//    TEST_F(RuntimeSettingsTest, Verbosity) {
//        EXPECT_EQ(rts->verbosity_level(), 2);
//    }

}
