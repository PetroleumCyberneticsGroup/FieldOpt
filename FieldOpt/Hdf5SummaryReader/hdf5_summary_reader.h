#ifndef FIELDOPT_HDF5SUMMARYREADER_H
#define FIELDOPT_HDF5SUMMARYREADER_H

#include <vector>
#include <H5Cpp.h>

class Hdf5SummaryReader {
public:
    Hdf5SummaryReader(const std::string file_path);
    const std::vector<double> &times() const { return times_; }


    int number_of_wells() const { return nwells_; }
    int number_of_tsteps() const { return ntimes_; }

    int well_type(const int well_number) const;
    bool is_injector(const int well_number) const;

    int number_of_perforations(const int well_number) const;
    int number_of_phases(const int well_number) const;
//    const std::vector<double> &

private:
    const H5std_string GROUP_NAME_RESTART;
    const H5std_string DATASET_NAME_TIMES;
    const H5std_string GROUP_NAME_FLOW_TRANSPORT;
    const H5std_string DATASET_NAME_WELL_STATES;

    typedef struct wstype_t {
        std::vector<double> vPressures;
        hvl_t vPressures_handle;
        std::vector<double> vTemperatures;
        hvl_t vTemperatures_handle;
        std::vector<double> vPhaseRates;
        hvl_t vPhaseRates_handle;
        std::vector<double> vPhaseRatesAtSC;
        hvl_t vPhaseRatesAtSC_handle;
        std::vector<double> vAverageDensity;
        hvl_t vAverageDensity_handle;
        std::vector<int> vIntData;
        hvl_t vIntData_handle;
        std::vector<double> vDoubleData;
        hvl_t vDoubleData_handle;
    } wstype_t;

    struct perforation_state {
        perforation_state(){}
        perforation_state(int nt);
        std::vector<double> pressures;
        std::vector<double> temperatures;
        std::vector<double> average_densities;
        std::vector<double> oil_rates;
        std::vector<double> water_rates;
        std::vector<double> gas_rates;
    };
    struct well_state {
        well_state(){}
        well_state(int nt, int np);
        int nperfs; //!< Number of perforations in the well
        int nphases; //!< Number of fluid phases
        std::vector<perforation_state> perforation_states;
        std::vector<int> well_types;
        std::vector<int> phase_status;
        std::vector<int> well_controls;
        std::vector<double> bottom_hole_pressures;
        std::vector<double> oil_rates_sc;
        std::vector<double> water_rates_sc;
        std::vector<double> gas_rates_sc;
        bool is_injector() const { return well_types[0] == 1; }
    };

    std::vector<double> readTimeVector(std::string file_path);
    void readWellStates(std::string file_path);
    void parseWsVector(std::vector<wstype_t> &wsvec);
    well_state parseWellState(std::vector<wstype_t> &ws, int wnr);

    int nwells_; //!< Number of wells in summary.
    int ntimes_; //!< Number of time steps in the summary.
    std::vector<double> times_;

    std::vector<well_state> well_states_;
};


#endif //FIELDOPT_HDF5SUMMARYREADER_H
