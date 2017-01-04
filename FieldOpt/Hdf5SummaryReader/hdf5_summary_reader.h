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
 * Note also that when you get the rates for an injector using the get_*_rates methods, the numbers will be negative,
 * whereas if you get them using the get_*_injection_rates methods, they will be positive.
 *
 * \todo This must also be tested for a 3 phase black oil model, it has only been tested for 2 phase dead oil.
 */
class Hdf5SummaryReader {
public:
    /*!
     * Read the HDF5 summary file written by AD-GPRS at the specified path.
     *
     * \note The provided path is not checked by this method, and should therefore be
     * checked before invoking this.
     * @param file_path Path to a .H5 summary file.
     * @return A Hdf5SummaryReader object containing information from the summary.
     */
    Hdf5SummaryReader(const std::string file_path);

    /*!
     * Get the vector containing all time steps in the summary.
     */
    const std::vector<double> &times_steps() const { return times_; }

    /*!
     * Get reservoir pressure vector.
     */
    std::vector<std::vector<double>> reservoir_pressure() const { return pressure_; }

    /*!
     * Get sgas vector.
     */
    std::vector<std::vector<double>> sgas() const { return sgas_; }

    /*!
     * Get soil vector.
     */
    std::vector<std::vector<double>> soil() const { return soil_; }

    /*!
     * Get swat vector.
     */
    std::vector<std::vector<double>> swat() const { return swat_; }

    /*!
     * Return vector of active grid cells.
     */
    const std::vector<int> &cells_active() { return cells_active_; }

    /*!
     * Return vector of active grid cell indices.
     */
    const std::vector<int> &cells_active_idx() { return cells_active_idx_; }

    /*!
     * Get total number of grid cells in model.
     */
    int cells_total_num() const { return cells_total_num_; }

    /*!
     * Get total number of active grid cells in model.
     */
    int cells_num_active() const { return cells_num_active_; }

    /*!
     * Get total number of inactive grid cells in model.
     */
    int cells_num_inactive() const { return cells_num_inactive_; }

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
     * Get the number of phases in the model.
     */
    int number_of_phases() const;

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

    /*!
     * Get the cumulative oil production (at standard conditions) at each time steps for a well.
     */
    std::vector<double> cumulative_oil_production_sc(const int well_number) const;

    /*!
     * Get the cumulative water production (at standard conditions) at each time steps for a well.
     */
    std::vector<double> cumulative_water_production_sc(const int well_number) const;

    /*!
     * Get the cumulative gas production (at standard conditions) at each time steps for a well.
     */
    std::vector<double> cumulative_gas_production_sc(const int well_number) const;

    /*!
     * Get field-wide oil production rates (at standard conditions) at each time step.
     */
    std::vector<double> field_oil_rates_sc() const;

    /*!
     * Get field-wide water production rates (at standard conditions) at each time step.
     */
    std::vector<double> field_water_rates_sc() const;

    /*!
     * Get field-wide gas production rates (at standard conditions) at each time step.
     */
    std::vector<double> field_gas_rates_sc() const;

    /*!
     * Get field-wide cumulative oil production (at standard conditions) at each time step.
     */
    std::vector<double> field_cumulative_oil_production_sc() const;

    /*!
     * Get field-wide cumulative water production (at standard conditions) at each time step.
     */
    std::vector<double> field_cumulative_water_production_sc() const;

    /*!
     * Get field-wide cumulative gas production (at standard conditions) at each time step.
     */
    std::vector<double> field_cumulative_gas_production_sc() const;

    /*!
     * Get the water injection rates (at standard conditions) at each time step for a well.
     */
    std::vector<double> water_injection_rates_sc(const int well_number) const;

    /*!
     * Get the gas injection rates (at standard conditions) at each time step for a well.
     */
    std::vector<double> gas_injection_rates_sc(const int well_number) const;

    /*!
     * Get the cumulative water injection (at standard conditions) at each time step for a well.
     */
    std::vector<double> cumulative_water_injection_sc(const int well_number) const;

    /*!
     * Get the cumulative gas injection (at standard conditions) at each time step for a well.
     */
    std::vector<double> cumulative_gas_injection_sc(const int well_number) const;

    /*!
     * Get the field-wide water injection rates (at standard conditions) at each time step.
     */
    std::vector<double> field_water_injection_rates_sc() const;

    /*!
     * Get the field-wide gas injection rates (at standard conditions) at each time step.
     */
    std::vector<double> field_gas_injection_rates_sc() const;

    /*!
     * Get the field-wide water injection cumulatives (at standard conditions) at each time step.
     */
    std::vector<double> field_cumulative_water_injection_sc() const;

    /*!
     * Get the field-wide gas injection cumulatives (at standard conditions) at each time step.
     */
    std::vector<double> field_cumulative_gas_injection_sc() const;



private:
    const H5std_string GROUP_NAME_RESTART; //!< The name of the restart group in the HDF5 file.
    const H5std_string DATASET_NAME_TIMES; //!< The name of the dataset containing the time step vector in the HDF5 file.
    const H5std_string GROUP_NAME_FLOW_TRANSPORT; //!< The name of the flow transport group in the HDF5 file.
    const H5std_string DATASET_NAME_WELL_STATES; //!< The name of the dataset containing the well states in the HDF5 file.
    const H5std_string DATASET_NAME_ACTIVE_CELLS; //!< The name of the dataset containing active cells vector.
    const H5std_string DATASET_NAME_PRESSURE; //!< The name of the dataset containing pressure and component data.
    const H5std_string DATASET_NAME_SATURATION; //!< The name of the dataset containing saturation data.
    /*!
     * The wstype_t datatype represents the datatype in which well states are stored in the HDF5 file.
     * Each element in the dataset contains information about a well and its perforations at a specific
     * time step.
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

    /*!
     * The perforation_data struct holds vectors containing rate, pressure, temperature and density
     * values at all time steps for a specific perforation in a well.
     */
    struct perforation_data {
        perforation_data(){}
        perforation_data(int nt);
        std::vector<double> pressures;
        std::vector<double> temperatures;
        std::vector<double> average_densities;
        std::vector<double> oil_rates;
        std::vector<double> water_rates;
        std::vector<double> gas_rates;
    };

    /*!
     * The well_data struct holds information about a specific well, as well as a vector of
     * perforation_data for all of its perforations, and vectors containing rate and pressure
     * values at all time steps.
     */
    struct well_data {
        well_data(){}
        well_data(int nt, int np);
        int nperfs; //!< Number of perforations in the well
        int nphases; //!< Number of fluid phases
        std::vector<perforation_data> perforation_states;
        std::vector<int> well_types;
        std::vector<int> phase_status;
        std::vector<int> well_controls;
        std::vector<double> bottom_hole_pressures;
        std::vector<double> oil_rates_sc;
        std::vector<double> water_rates_sc;
        std::vector<double> gas_rates_sc;
        bool is_injector() const { return well_types[0] == 1; }
    };

    void readTimeVector(std::string file_path); //!< Read the time vector from the HDF5 summary file.
    void readWellStates(std::string file_path); //!< Read all well state information from the HDF5 summary file.
    void parseWsVector(std::vector<wstype_t> &wsvec); //!< Populate well_states_ by creating well_data and perforation_data objects from the wstype_t vector.
    well_data parseWellState(std::vector<wstype_t> &ws, int wnr); //!< Parse the states for a single well and create a well_data object.

    void readActiveCells(std::string file_path); //!< Read vector defining which cells are active from the HDF5 summary file.
    void readReservoirPressure(std::string file_path); //!< Read reservoir cell pressures for each time step.
    void readSaturation(std::string file_path); //!< Read cell saturations for each time step.

    /*!
     * variables containing information about the reservoir cell ensemble,
     * e.g., number of active cells, corresponding active cell indices, etc.
     */
    std::vector<int> cells_all_vector_; //!< Vector def. status for all cells (size equal to total number of cells).
    std::vector<int> cells_active_, cells_inactive_;
    std::vector<int> cells_active_idx_, cells_inactive_idx_;
    int cells_total_num_; //!< Total number of grid cells in model.
    int cells_num_active_; //!< Total number of active grid cells in model.
    int cells_num_inactive_; //!< Total number of inactive grid cells in model.

    int cells_find_statuses(std::vector<int> &cells_all_vector_); //!< Fills inactive/active cells numbers and indices

    int nwells_; //!< Number of wells in summary.
    int ntimes_; //!< Number of time steps in the summary.
    int nphases_; //!< Number of phases in the model.

    std::vector<double> times_; //!< Vector containing all time steps.
    std::vector<std::vector<double>> pressure_; //!< Vector containing reservoir pressures.
    std::vector<std::vector<double>> soil_; //!< Vector containing oil saturation.
    std::vector<std::vector<double>> sgas_; //!< Vector containing gas saturation.
    std::vector<std::vector<double>> swat_; //!< Vector containing water saturation.

    std::vector<std::vector<double>> getSaturation(H5::Group dataset, hsize_t sat_type);

    /*!
     * Calculate the cumulative using the composite trapezoidal rule.
     *
     * The standard form is
     * \f[
     *  T(f, h) = \frac{h}{2} \sum_{k=1}^{M}\left[ f(x_{k-1}) + f(x_k) \right]
     * \f]
     *
     * In our case, \f$ h \f$ (the time difference) is not constant but can vary from step to step in the time step
     * vector \f$ t \f$, so \f$ h = f(k) = t_k - t_{k-1} \f$. Additionally, \f$ f(x_k) = r_k \f$ where \f$ r \f$ is
     * the rate vector. We also need to compute the cumulative at various time steps, so \f$ M \f$ is a variable.
     * So then we have
     * \f[
     *  T(M) = \sum_{k=1}^M \left[ \frac{t_k - t_{k-1}}{2} \left( r_{k-1} + r_k \right) \right]
     * \f]
     * and the cumulative vector becomes
     * \f[
     *  c = [0, T(1), T(2), ..., T(N)]
     * \f]
     * where \f$ N \f$ is the number of time steps.
     * @param rates
     * @return
     */
    std::vector<double> calculate_cumulative(const std::vector<double> &rates) const;

    std::vector<well_data> well_states_; //!< Vector containing all information from the well states dataset.
};


#endif //FIELDOPT_HDF5SUMMARYREADER_H
