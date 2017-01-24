#include "../hdf5_summary_reader.h"
#include <gtest/gtest.h>
#include <hdf5_summary_reader.h>

namespace {
    class Hdf5SummaryReaderTest : public ::testing::Test {
    public:

        Hdf5SummaryReaderTest() {}

    protected:
        virtual void SetUp() {}
        virtual void TearDown() {}
        std::string file_path = "../examples/ADGPRS/5spot/5SPOT.vars.h5";
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

        std::vector<double> expected_times{0.0, 1.0, 6.0, 31.0, 50, 65.0, 95.0, 100.0};
        for (int i = 0; i < times.size(); ++i) {
            EXPECT_DOUBLE_EQ(expected_times[i], times[i]);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, ActiveCellVector) {
        auto reader = Hdf5SummaryReader(file_path);

           auto cells_total_num_ = reader.cells_total_num();
          auto cells_num_active_ = reader.cells_num_active();
        auto cells_num_inactive_ = reader.cells_num_inactive();

            auto cells_active_ = reader.cells_active();
        auto cells_active_idx_ = reader.cells_active_idx();

        // Test based on all cells in test reservoir (5spot) being active
        EXPECT_EQ(cells_total_num_, 3600);
        EXPECT_EQ(cells_num_active_, cells_total_num_);
        EXPECT_EQ(cells_num_inactive_, 0);

        for (int i = 0; i < cells_num_active_; ++i) {
            // std::cout << cells_num_active_[i] << std::endl;
            EXPECT_EQ(cells_active_[i], i);
            EXPECT_EQ(cells_active_idx_[i], i);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, readReservoirPressure) {
          auto reader = Hdf5SummaryReader(file_path);
        auto pressure = reader.reservoir_pressure();

        std::vector<double> def_pressure = {
            370.555603, 370.55451121327735, 366.80490627652864,
            281.07546615360917, 213.85427339240036, 179.59397120055957,
            154.40334284097705, 151.17045510746573
        };

        // Test against first component of each pressure vector
        // to confirm we're reading correct column and that the
        // read vector has been correctly resized
        for (int i = 0; i < pressure.size(); ++i) {
            EXPECT_EQ(def_pressure[i], pressure[i][0]);
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
                { 5.33392e-07, 1.28479e+00, 1.93622e+00, 3.34471e+00, 4.12568e+00, 4.32977e+00, 4.44898e+00, 4.46081e+00 },
                { 4.47823e+03, 1.44194e+03, 1.63272e+03, 1.29462e+03, 1.07743e+03, 9.59778e+02, 7.88312e+02, 7.61999e+02 },
                { -4.58699e+02,  1.82363e+01,  2.49221e+02,  6.14325e+02,  7.30089e+02,  7.73242e+02,  7.87443e+02,  7.87746e+02 },
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
                { -3.96017e+04, -1.69272e+04, -1.17907e+04, -7.56994e+03, -6.67773e+03, -6.42420e+03, -6.28320e+03, -6.26953e+03 },
                { 4.04222e+03, 2.10610e+03, 1.38645e+03, 6.72054e+02, 4.92830e+02, 4.31293e+02, 3.95274e+02, 3.90997e+02 },
                { 3.39375e-07, 1.09275e-07, 1.92567e-01, 5.78575e+02, 1.07060e+03, 1.34168e+03, 1.68984e+03, 1.75021e+03 },
                { -9.47215e+01, 1.38201e-09, 1.88868e-08, 4.65556e-08, 5.53286e-08, 5.85988e-08, 9.65374e-01, 1.26232e+00 },
                { 1.26504e+04, 3.21890e+03, 1.62203e+03, 1.22582e+03, 1.22453e+03, 1.23722e+03, 1.24947e+03, 1.25108e+03 }
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
        double expected_final_cumulatives[5] = {-797676, 69898, 95067, -27, 140928};
        for (int w = 0; w < reader.number_of_wells(); ++w) {
            EXPECT_NEAR(expected_final_cumulatives[w], reader.cumulative_water_production_sc(w)[reader.number_of_tsteps()-1], 5.1);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, CumulativeOilProductionSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_final_cumulatives[5] = {0.0 , 363, 115149, 62637.4295, 0.0 };
        for (int w = 0; w < reader.number_of_wells(); ++w) {
            EXPECT_NEAR(expected_final_cumulatives[w], reader.cumulative_oil_production_sc(w)[reader.number_of_tsteps()-1], 2.0);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, FieldOilRatesSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_rates[8] = {4019, 1461, 1883, 1912, 1811, 1737, 1580, 1554};
        for (int t = 0; t < reader.number_of_tsteps(); ++t) {
            EXPECT_NEAR(expected_rates[t], reader.field_oil_rates_sc()[t], 1.0);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, FieldOilCumulativesSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_values[8] = {0.0, 2740, 11103, 58555, 93933, 120550, 170314, 178150};
        for (int t = 0; t < reader.number_of_tsteps(); ++t) {
            EXPECT_NEAR(expected_values[t], reader.field_cumulative_oil_production_sc()[t], 1.0);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, WaterInjectionRatesSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_values[8] = {3.96017e+04, 1.69272e+04, 1.17907e+04, 7.56994e+03, 6.67773e+03, 6.42420e+03, 6.28320e+03, 6.26953e+03};
        EXPECT_THROW(reader.water_injection_rates_sc(1), std::runtime_error);
        for (int t = 0; t < reader.number_of_tsteps(); ++t) {
            EXPECT_NEAR(expected_values[t], reader.water_injection_rates_sc(0)[t], 1.0);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, FieldWaterInjectionRatesSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_values[8] = {-3.96017e+04, -1.69272e+04, -1.17907e+04, -7.56994e+03, -6.67773e+03, -6.42420e+03, -6.28320e+03, -6.26953e+03};
        EXPECT_THROW(reader.water_injection_rates_sc(1), std::runtime_error);
        for (int t = 0; t < reader.number_of_tsteps(); ++t) {
            EXPECT_NEAR(-1 * expected_values[t], reader.field_water_injection_rates_sc()[t], 1.0);
        }
    }

    TEST_F(Hdf5SummaryReaderTest, FieldCumulativeWaterInjectionSC) {
        auto reader = Hdf5SummaryReader(file_path);
        double expected_values[8] = {0, 28264, 100059, 342066, 477419, 575683, 766294, 797676};
        for (int t = 0; t < reader.number_of_tsteps(); ++t) {
            EXPECT_NEAR(expected_values[t], reader.field_cumulative_water_injection_sc()[t], 1.0);
        }
    }
}
