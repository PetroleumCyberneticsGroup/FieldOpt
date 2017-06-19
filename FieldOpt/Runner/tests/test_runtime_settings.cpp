#include <gtest/gtest.h>
#include "Runner/runtime_settings.h"

namespace {

    class RuntimeSettingsTest : public testing::Test {
        int argc = 16;
        const char *argv[16] = {"FieldOpt",
                        "/home/einar/git/pcg/FieldOpt/examples/ADGPRS/5spot/fo_driver_5vert_wells.json",
                        "/home/einar/fieldopt_output",
                        "-g", "/home/einar/git/pcg/FieldOpt/examples/Flow/5spot/5SPOT.EGRID",
                        "-s", "/home/einar/git/pcg/FieldOpt/examples/Flow/5spot/5SPOT.DATA",
                        "-b", "/home/einar/git/pcg/FieldOpt/FieldOpt/cmake-build-debug/bin",
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
