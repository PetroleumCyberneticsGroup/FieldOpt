#include <gtest/gtest.h>
#include "Runner/runtime_settings.h"
#include "globals.hpp"

namespace {

    class RuntimeSettingsTest : public testing::Test {
        int argc = 15;
        const char *argv[15] = {"FieldOpt",
                        "/home/einar/Documents/GitHub/PCG/FieldOpt/examples/ADGPRS/5spot/fo_driver_5vert_wells.json",
                        "/home/einar/fieldopt_output",
                        "-g", "/home/einar/Documents/GitHub/PCG/FieldOpt/examples/Flow/5spot/5SPOT.EGRID",
                        "-s", "/home/einar/Documents/GitHub/PCG/FieldOpt/examples/Flow/5spot/5SPOT.DATA",
                        "-b", "/home/einar/.CLion2016.2/system/cmake/generated/FieldOpt-c9373114/c9373114/Debug/bin",
                        "-f",
                        "-v",
                        "-t", "1000",
                        "-r", "mpisync"
        };


    protected:

        RuntimeSettingsTest() {
            rts = new Runner::RuntimeSettings(argc, argv);
        }
        ~RuntimeSettingsTest(){}
        Runner::RuntimeSettings *rts;
    };

    TEST_F(RuntimeSettingsTest, Verbosity) {
        EXPECT_EQ(VERBOSITY, -1);
    }

}
