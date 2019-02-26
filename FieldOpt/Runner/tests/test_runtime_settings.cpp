#include <gtest/gtest.h>
#include "Runner/runtime_settings.h"
#include "Settings/tests/test_resource_example_file_paths.hpp"

namespace {

    class RuntimeSettingsTest : public testing::Test {
        int argc = 16;
        const char *argv[16] = {"FieldOpt",
                        TestResources::ExampleFilePaths::driver_5spot_.c_str(),
                        TestResources::ExampleFilePaths::directory_output_.c_str(),
                        "-g", TestResources::ExampleFilePaths::grid_flow_5spot_.c_str(),
                        "-s", TestResources::ExampleFilePaths::deck_flow_5spot_.c_str(),
                        "-b", ".",
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
