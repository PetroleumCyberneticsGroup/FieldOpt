#include "../hdf5_summary_reader.h"
#include <gtest/gtest.h>

namespace {
    class Hdf5SummaryReaderTest : public ::testing::Test {
    public:

        Hdf5SummaryReaderTest() {}

    protected:
        virtual void SetUp() {}
        virtual void TearDown() {}
        std::string file_path = "/home/einar/Documents/GitHub/PCG/FieldOpt/examples/ADGPRS/5spot/5SPOT.SIM.H5"; // \todo Use the path from TestResources
    };


    TEST_F(Hdf5SummaryReaderTest, Constructor) {
        auto reader = Hdf5SummaryReader(file_path);
    }


    TEST_F(Hdf5SummaryReaderTest, TimeVector) {
        auto reader = Hdf5SummaryReader(file_path);
        std::vector<double> times = reader.get_times();
        EXPECT_EQ(8, times.size());

        std::vector<double> expected_times{0.0, 3.0, 18.0, 48.0, 50.0, 65.0, 95.0, 100.0};
        for (int i = 0; i < times.size(); ++i) {
            EXPECT_FLOAT_EQ(expected_times[i], times[i]);
        }
    }
}
