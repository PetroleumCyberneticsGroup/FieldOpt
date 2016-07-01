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
        EXPECT_EQ(5, reader.number_of_wells());
        EXPECT_EQ(8, reader.number_of_tsteps());
    }


    TEST_F(Hdf5SummaryReaderTest, TimeVector) {
        auto reader = Hdf5SummaryReader(file_path);
        std::vector<double> times = reader.times();
        EXPECT_EQ(8, times.size());

        std::vector<double> expected_times{0.0, 3.0, 18.0, 48.0, 50.0, 65.0, 95.0, 100.0};
        for (int i = 0; i < times.size(); ++i) {
            EXPECT_FLOAT_EQ(expected_times[i], times[i]);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, IntegerData) {
        auto reader = Hdf5SummaryReader(file_path);
        int expected_types[5] = {1, -1, -1, -1, -1};
        int expected_nperfs[5] = {1, 1, 1, 1, 1};
        int expected_nphases[5] = {2, 2, 2, 2, 2};
        for (int w = 0; w < reader.number_of_wells(); ++w) {
            EXPECT_EQ(expected_types[w], reader.well_type(w));
            if (w == 0) EXPECT_TRUE(reader.is_injector(w));
            else EXPECT_FALSE(reader.is_injector(w));

            EXPECT_EQ(expected_nperfs[w], reader.number_of_perforations(w));
            EXPECT_EQ(expected_nphases[w], reader.number_of_phases(w));
        }
    }
}
//struct well_state {
//    well_state(){}
//    well_state(int nt, int np);
//    int nperfs; //!< Number of perforations in the well
//    int nphases; //!< Number of fluid phases
//    std::vector<perforation_state> perforation_states;
//    std::vector<int> well_types;
//    std::vector<int> phase_status;
//    std::vector<int> well_controls;
//    std::vector<double> bottom_hole_pressures;
//    std::vector<double> oil_rates_sc;
//    std::vector<double> water_rates_sc;
//    std::vector<double> gas_rates_sc;
//};
//struct perforation_state {
//    perforation_state(){}
//    perforation_state(int nt);
//    std::vector<double> pressures;
//    std::vector<double> temperatures;
//    std::vector<double> average_densities;
//    std::vector<double> oil_rates;
//    std::vector<double> water_rates;
//    std::vector<double> gas_rates;
//};
