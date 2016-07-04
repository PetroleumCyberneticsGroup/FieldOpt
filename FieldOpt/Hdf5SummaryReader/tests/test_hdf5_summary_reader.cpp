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
        EXPECT_EQ(2, reader.number_of_phases());
    }


    TEST_F(Hdf5SummaryReaderTest, TimeVector) {
        auto reader = Hdf5SummaryReader(file_path);
        std::vector<double> times = reader.times_steps();
        EXPECT_EQ(8, times.size());

        std::vector<double> expected_times{0.0, 3.0, 18.0, 48.0, 50.0, 65.0, 95.0, 100.0};
        for (int i = 0; i < times.size(); ++i) {
            EXPECT_DOUBLE_EQ(expected_times[i], times[i]);
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

    TEST_F(Hdf5SummaryReaderTest, BottomHolePressures) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_bhps[5][8] = {
                { 230, 230, 230, 230, 230, 230, 230, 230 },
                { 90, 90, 90, 90, 90, 90, 90, 90 },
                { 90, 90, 90, 90, 90, 90, 90, 90 },
                { 90, 90, 90, 90, 90, 90, 90, 90 },
                { 90, 90, 90, 90, 90, 90, 90, 90 }
        };
        for (int w = 0; w < reader.number_of_wells(); ++w) {
            EXPECT_EQ(reader.number_of_tsteps(), reader.bottom_hole_pressures(w).size());
            for (int t = 0; t < reader.number_of_tsteps(); ++t) {
                EXPECT_DOUBLE_EQ(expected_bhps[w][t], reader.bottom_hole_pressures(w)[t]);
            }
        }
    }

    TEST_F(Hdf5SummaryReaderTest, OilRatesAtSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_oil_rates[5][8] = {
                { -0, -0, -0, -0, -0, -0, -0, -0 },
                { 0, 1.62626, 2.6165, 3.99704, 4.04895, 4.29621, 4.44254, 4.45752 },
                { 4478.23, 1527.28, 1472.31, 1106.74, 1084.17, 963.563, 789.691, 763.152 },
                { -458.699, 127.526, 491.173, 711.614, 724.54, 770.369, 786.024, 786.486 },
                { 0, 0, 0, 0, 0, 0, 0, 0 }
        };
        for (int w = 0; w < reader.number_of_wells(); ++w) {
            EXPECT_EQ(reader.number_of_tsteps(), reader.oil_rates_sc(w).size());
            for (int t = 0; t < reader.number_of_tsteps(); ++t) {
                EXPECT_NEAR(expected_oil_rates[w][t], reader.oil_rates_sc(w)[t], 0.05);
            }
        }
    }

    TEST_F(Hdf5SummaryReaderTest, WaterRatesAtSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_water_rates[5][8] = {
                { -39601.7, -14233.9, -8755.69, -6803.42, -6701.8, -6431.61, -6282.44, -6267.97 },
                { 4042.22, 1734.34, 890.349, 519.612, 500.087, 434.554, 396.248, 391.699 },
                { 0, 0, 239.944, 991.767, 1051.24, 1328.3, 1682.86, 1744.34 },
                { -94.7215, 0, 0, 0, 0, 0, 1.02276, 1.33214 },
                { 12650.4, 2282.57, 1284.14, 1223.85, 1223.44, 1236.45, 1249.26, 1250.94 }
        };
        for (int w = 0; w < reader.number_of_wells(); ++w) {
            EXPECT_EQ(reader.number_of_tsteps(), reader.water_rates_sc(w).size());
            for (int t = 0; t < reader.number_of_tsteps(); ++t) {
                EXPECT_NEAR(expected_water_rates[w][t], reader.water_rates_sc(w)[t], 0.05);
            }
        }
    }

    TEST_F(Hdf5SummaryReaderTest, CumulativWaterProductionSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_final_cumulatives[5] = {-820650, 71961, 93900, -120.8536, 151200};
        for (int w = 0; w < reader.number_of_wells(); ++w) {
            EXPECT_NEAR(expected_final_cumulatives[w], reader.cumulative_water_production_sc(w)[reader.number_of_tsteps()-1], 5.1);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, CumulativeOilProductionSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_final_cumulatives[5] = {0.0 , 357.42928, 117920.765, 62110.4295, 0.0 };
        for (int w = 0; w < reader.number_of_wells(); ++w) {
            EXPECT_NEAR(expected_final_cumulatives[w], reader.cumulative_oil_production_sc(w)[reader.number_of_tsteps()-1], 1.0);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, FieldOilRatesSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_rates[8] = {4019.531, 1656.43226, 1966.0995, 1822.35104, 1812.75895, 1738.22821, 1580.15754, 1554.09552};
        for (int t = 0; t < reader.number_of_tsteps(); ++t) {
            EXPECT_NEAR(expected_rates[t], reader.field_oil_rates_sc()[t], 1.0);
        }
    }


    TEST_F(Hdf5SummaryReaderTest, FieldWaterRatesSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_rates[8] = {16597.8985, 4016.91, 2414.433, 2735.229, 2774.767, 2999.304, 3329.39076, 3388.31114};
        for (int t = 0; t < reader.number_of_tsteps(); ++t) {
            EXPECT_NEAR(expected_rates[t], reader.field_water_rates_sc()[t], 1.0);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, FieldOilCumulativesSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_values[8] = {0.0, 8513.94489, 35682.93309, 92509.69119, 96144.80118, 122777.20488, 172552.99113, 180388.62378};
        for (int t = 0; t < reader.number_of_tsteps(); ++t) {
            EXPECT_NEAR(expected_values[t], reader.field_cumulative_oil_production_sc()[t], 1.0);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, FieldWaterCumulativesSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_values[8] = {0.0, 30922.21275, 79157.28525, 156402.21525, 161912.21125, 205217.74375, 300148.16515, 316942.4199};
        for (int t = 0; t < reader.number_of_tsteps(); ++t) {
            EXPECT_NEAR(expected_values[t], reader.field_cumulative_water_production_sc()[t], 1.0);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, WaterInjectionRatesSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_values[8] = {39601.7, 14233.9, 8755.69, 6803.42, 6701.8, 6431.61, 6282.44, 6267.97};
        EXPECT_THROW(reader.water_injection_rates(1), std::runtime_error);
        for (int t = 0; t < reader.number_of_tsteps(); ++t) {
            EXPECT_NEAR(expected_values[t], reader.water_injection_rates(0)[t], 1.0);
        }
    }
}
