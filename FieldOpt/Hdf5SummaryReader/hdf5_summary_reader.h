#ifndef FIELDOPT_HDF5SUMMARYREADER_H
#define FIELDOPT_HDF5SUMMARYREADER_H

#include <vector>
#include <H5Cpp.h>

/*!
 * The Hdf5SummaryReader class reads the summaries written in the HDF5 format by the AD-GPRS reservoir simulator.
 *
 * Currently the class supports getting rates and cumulatives for each well and for the field, as well as
 * bottom hole pressures and well type for each well.
 *
 * Note that we do not have access to the well names in the summary, thus we use well numbers. This number
 * corresponds to the row on which information about the well is found. This number apparently does not correspond to
 * the order in which the wells were specified in the driver file, thus we have no way of distinguishing wells other
 * that by producer/injector. Note also that we do not know whether the order in which the wells appear can change
 * between subsequent runs of the simulator.
 *
 * \todo This must also be tested for a 3 phase black oil model, it has only been tested for 2 phase dead oil.
 */
class Hdf5SummaryReader {
public:
    /*!
     * Read the HDF5 summary file written by AD-GPRS at the specified path.
     *
     * \note The provided path is not checked by this method, and should therefore be checked before
     * invoking this.
     * @param file_path Path to a .H5 summary file.
     * @return A Hdf5SummaryReader object containing information from the summary.
     */
    Hdf5SummaryReader(const std::string file_path);

    /*!
     * Get the vector containing all time steps in the summary.
     */
    const std::vector<double> &times_steps() const { return times_; }

    /*!
     * Get the number of wells found in the summary.
     */
    int number_of_wells() const { return nwells_; }

    /*!
     * Get the number of time steps found in the summary.
     */
    int number_of_tsteps() const { return ntimes_; }

    /*!
     * Get the type reported for a well. +1 indicates an injector, -1 indicates a producer.
     */
    int well_type(const int well_number) const;

    /*!
     * Check if a well is reported as an injector.
     * @return True if it is an injector; otherwise false.
     */
    bool is_injector(const int well_number) const;

    /*!
     * Get the number of perforations reported for a well.
     */
    int number_of_perforations(const int well_number) const;

    /*!
     * Get the number of phases (water/oil/gas) found for a well.
     */
    int number_of_phases(const int well_number) const;

    /*!
     * Get the vector of bottom hole pressures at each time step for a well.
     */
    const std::vector<double> &bottom_hole_pressures(const int well_number) const;

    /*!
     * Get the vector of oil rates (at standard conditions) at each time step for a well.
     */
    const std::vector<double> &oil_rates_sc(const int well_number) const;

    /*!
     * Get the vector of water rates (at standard conditions) at each time step for a well.
     */
    const std::vector<double> &water_rates_sc(const int well_number) const;

    /*!
     * Get the vector of water rates (at standard conditions) at each time step for a well.
     */
    const std::vector<double> &gas_rates_sc(const int well_number) const;

private:
    const H5std_string GROUP_NAME_RESTART; //!< The name of the restart group in the HDF5 file.
    const H5std_string DATASET_NAME_TIMES; //!< The name of the dataset containing the time step vector in the HDF5 file.
    const H5std_string GROUP_NAME_FLOW_TRANSPORT; //!< The name of the flow transport group in the HDF5 file.
    const H5std_string DATASET_NAME_WELL_STATES; //!< The name of the dataset containing the well states in the HDF5 file.

    /*!
     * The wstype_t datatype represents the datatype in which well states are stored in the HDF5 file. Each element
     * in the dataset contains information about a well and its perforations at a specific time step.
     */
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
