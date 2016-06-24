#include <gtest/gtest.h>
#include <tests/test_resource_example_file_paths.h>
#include <Simulation/execution_scripts/execution_scripts.h>

#include "Utilities/unix/execution.h"

namespace {

    class UnixPipeTest : public ::testing::Test {
    protected:
        UnixPipeTest() {
            directory_ = "../fieldopt_output";
            command_ = "";
            //commands_ = QStringList();
            //commands_.append("ls");
            //commands_.append("touch test.txt");
            //commands_.append("ls");
            //commands_.append("rm test.txt");
            //commands_.append("ls");
        }
        virtual ~UnixPipeTest() {}

        virtual void SetUp() {}
        virtual void TearDown() {}

        QString directory_;
        QString command_;
        QStringList commands_;
    };

    TEST_F(UnixPipeTest, SingleCommand) {
        ::Utilities::Unix::Exec(directory_, command_, true);
    }

    TEST_F(UnixPipeTest, MultipleCommands) {
        ::Utilities::Unix::Exec(directory_, commands_, true);
    }

//    TEST_F(UnixPipeTest, TimeoutScript) {
//        QString test_script_path = "/home/einar/Documents/testpit/bash/timeout.sh"; // \todo Make this something that works for all
//        QStringList args = {"2", "Waited 2 seconds"};
//        EXPECT_FALSE(::Utilities::Unix::ExecShellScriptTimeout(test_script_path, args, 1));
//        EXPECT_TRUE(::Utilities::Unix::ExecShellScriptTimeout(test_script_path, args, 5));
//    }
//
//    TEST_F(UnixPipeTest, TimeoutSimulationAbort) {
//        QString test_script_path = "/home/einar/Documents/GitHub/PCG/FieldOpt/FieldOpt/execution_scripts/bash_adgprs.sh";
//        QString output_path = "/home/einar/fieldoptoutput2";
//        QString adgprs_driver = "5SPOT.gprs";
//        QStringList args = {output_path, adgprs_driver};
//        EXPECT_FALSE(::Utilities::Unix::ExecShellScriptTimeout(test_script_path, args, 1));
//    }
//
//    TEST_F(UnixPipeTest, TimeoutSimulationSuccess) {
//        QString test_script_path = "/home/einar/Documents/GitHub/PCG/FieldOpt/FieldOpt/execution_scripts/bash_adgprs.sh";
//        QString output_path = "/home/einar/fieldoptoutput2";
//        QString adgprs_driver = "5SPOT.gprs";
//        QStringList args = {output_path, adgprs_driver};
//        EXPECT_FALSE(::Utilities::Unix::ExecShellScriptTimeout(test_script_path, args, 1));
//        EXPECT_TRUE(::Utilities::Unix::ExecShellScriptTimeout(test_script_path, args, 5));
//        EXPECT_TRUE(::Utilities::Unix::ExecShellScriptTimeout(test_script_path, args, 10));
//        EXPECT_TRUE(::Utilities::Unix::ExecShellScriptTimeout(test_script_path, args, 15));
//    }
//
//    TEST_F(UnixPipeTest, TimeoutSimulationFail) {
//        QString test_script_path = "/home/einar/Documents/GitHub/PCG/FieldOpt/FieldOpt/execution_scripts/bash_adgprs.sh";
//        QString output_path = "/home/einar/fieldopt_output_unstable";
//        QString adgprs_driver = "5SPOT.gprs";
//        QStringList args = {output_path, adgprs_driver};
//        EXPECT_FALSE(::Utilities::Unix::ExecShellScriptTimeout(test_script_path, args, 10));
//        EXPECT_FALSE(::Utilities::Unix::ExecShellScriptTimeout(test_script_path, args, 10));
//    }
}
