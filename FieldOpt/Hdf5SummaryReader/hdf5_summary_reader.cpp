/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>
   Additions by M.Bellout (2017) <mathias.bellout@ntnu.no>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "hdf5_summary_reader.h"
#include <iostream>
#include <assert.h>
#include <boost/current_function.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>

using namespace H5;
Hdf5SummaryReader::Hdf5SummaryReader(const std::string file_path,
                                     bool get_cell_data,
                                     bool debug)
: GROUP_NAME_RESTART("RESTART"),
  DATASET_NAME_TIMES("TIMES"),
  GROUP_NAME_FLOW_TRANSPORT("FLOW_TRANSPORT"),
  DATASET_NAME_ACTIVE_CELLS("ACTIVE_CELLS"),
  DATASET_NAME_WELL_STATES("WELL_STATES"),
  DATASET_NAME_PRESSURE("PTZ"),
  DATASET_NAME_SATURATION("GRIDPROPTIME")
{
    readTimeVector(file_path);
    readWellStates(file_path);
    debug_ = debug;

    /*!
     * These are only called if we want to extract cell data from
     * the h5 file for postprocessing/visualization purposes
     */
    if (get_cell_data){
        readActiveCells(file_path);
        readReservoirPressure(file_path);
        readSaturation(file_path);
    }
}

void Hdf5SummaryReader::readSaturation(std::string file_path) {

    // Check file exists
    H5File file(file_path, H5F_ACC_RDONLY);
    Group group = Group(file.openGroup(GROUP_NAME_FLOW_TRANSPORT));
    auto dataset_exists = H5Lexists(group.getId(), "GRIDPROPTIME", H5F_ACC_RDONLY);
    std::vector<std::vector<double>> sgas, soil, swat;

    if (dataset_exists) {

        hsize_t SOIL, SWAT, SGAS;
        if (number_of_phases() < 3){
            SGAS = 0;
            SOIL = 2; // col: 3
            SWAT = 1; // col: 2
        }else{
            SGAS = 1; // col: 2
            SOIL = 2; // col: 3
            SWAT = 3; // col: 4
        }

        sgas_ = getSaturation(group, SGAS);
        soil_ = getSaturation(group, SOIL);
        swat_ = getSaturation(group, SWAT);

    }else{

        sgas_ = reservoir_pressure();
        soil_ = reservoir_pressure();
        swat_ = reservoir_pressure();
    }
}

std::vector<std::vector<double>> Hdf5SummaryReader::getSaturation(
        Group group, hsize_t sat_type) {

    // Read the file
    DataSet dataset = DataSet(group.openDataSet(DATASET_NAME_SATURATION));
    DataSpace dataspace = dataset.getSpace();
    hsize_t dims[3];
    dataspace.getSimpleExtentDims(dims, NULL);

    std::vector<double> vector;
    vector.resize(dims[0] * dims[2]);
    std::vector<std::vector<double>>  sat_; //!< Temporary saturation vector.

    if (sat_type > 0){
        // Define hyperslab
        hsize_t count[3] = {dims[0], 1, dims[2]};
        hsize_t offset[3] = {0, sat_type, 0};
        dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);

        // Size of selected column + define memory space
        hsize_t col_sz[2] = {dims[0], dims[2]};
        DataSpace mspace(2, col_sz);

        dataset.read(vector.data(), PredType::NATIVE_DOUBLE, mspace, dataspace);
    }else{
        vector.resize(dims[0] * dims[2],0);
    }

    int ncells = (int)dims[0];
    int ntime_steps = (int)dims[2];

    for (int tt = 0; tt < ntime_steps; ++tt) {
        std::vector<double> sub;
        for (int cc = 0; cc < ncells; ++cc) {
            sub.push_back(vector[ cc * ntime_steps + tt ]);
        }
        sat_.push_back(sub);
    }

    return sat_;
}

void Hdf5SummaryReader::readReservoirPressure(std::string file_path) {

    // Read the file
    H5File file(file_path, H5F_ACC_RDONLY);
    Group group = Group(file.openGroup(GROUP_NAME_FLOW_TRANSPORT));
    DataSet dataset = DataSet(group.openDataSet(DATASET_NAME_PRESSURE));

    DataSpace dataspace = dataset.getSpace();
    hsize_t dims[3];

    auto rank = dataspace.getSimpleExtentDims(dims, NULL);
    if (debug_){
        std::cout << "[\033[1;33m" << BOOST_CURRENT_FUNCTION << ":\033[0m\n"
                  << "dataset rank " << rank << ", dims "
                  << (unsigned long)(dims[0]) << " x "
                  << (unsigned long)(dims[1]) << " x "
                  << (unsigned long)(dims[2]) << std::endl;
    }

    // Define hyperslab
    hsize_t count[3] = {dims[0], 1, dims[2]};
    hsize_t offset[3] = {0, 0, 0};
    dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);

    // Size of selected column + define memory space
    hsize_t col_sz[2] = {dims[0], dims[2]};
    DataSpace mspace(2, col_sz);

    std::vector<double> vector;
    vector.resize(dims[0] * dims[2]);
    dataset.read(vector.data(), PredType::NATIVE_DOUBLE, mspace, dataspace);

    // Reorder pressure vector
    // Note:
    // Data/time component ordering inside data vector:
    // Example: pressure vector with 5 cells over 8 time steps:
    // size of vector = ncells (x ndata_cols=1) x ntime_steps
    //
    //        cell 1   cell 2   cell 3   cell 4   cell 5
    //time  |--------|--------|--------|--------|--------|
    //steps: 12345678 12345678 12345678 12345678 12345678

         int ncells = (int)dims[0];
     int ndata_cols = (int)dims[1];
    int ntime_steps = (int)dims[2];

    for (int tt = 0; tt < ntime_steps; ++tt) {
        std::vector<double> sub;
        for (int cc = 0; cc < ncells; ++cc) {
            sub.push_back(vector[ cc * ntime_steps + tt ]);
        }
        pressure_.push_back(sub);
    }
}

void Hdf5SummaryReader::readActiveCells(std::string file_path) {

    // Read the file
    H5File file(file_path, H5F_ACC_RDONLY);
    Group group = Group(file.openGroup(GROUP_NAME_FLOW_TRANSPORT));
    DataSet dataset = DataSet(group.openDataSet(DATASET_NAME_ACTIVE_CELLS));

    DataSpace dataspace = dataset.getSpace();
    hsize_t dims[2];

    // rank variable can be used for debug
    auto rank = dataspace.getSimpleExtentDims(dims, NULL);
    if (debug_){
        std::cout << "[\033[1;33m" << BOOST_CURRENT_FUNCTION << ":\033[0m\n"
                  << "dataset rank " << rank << ", dims "
                  << (unsigned long)(dims[0]) << " x "
                  << (unsigned long)(dims[1]) << std::endl;
    }

    // Define hyperslab
    hsize_t  count[2] = { dims[0], 1 };
    hsize_t offset[2] = { 0, 0 };
    dataspace.selectHyperslab( H5S_SELECT_SET, count, offset );

    // Size of selected colum + define memory space
    hsize_t col_sz[1] = { dims[0] };
    DataSpace mspace( 1, col_sz );

    // Read and reorder vector
    std::vector<int> vector;
    vector.resize(dims[0]);
    dataset.read(vector.data(), PredType::NATIVE_INT, mspace, dataspace);

    cells_all_vector_ = vector;
    cells_find_statuses(cells_all_vector_);
}

void Hdf5SummaryReader::readTimeVector(std::string file_path) {
    // Read the file
    H5File file(file_path, H5F_ACC_RDONLY);
    Group group = Group(file.openGroup(GROUP_NAME_RESTART));
    DataSet dataset = DataSet(group.openDataSet(DATASET_NAME_TIMES));

    // Check that the type is correct
    H5T_class_t type_class = dataset.getTypeClass();
    if (type_class != H5T_FLOAT)
        throw std::runtime_error("Unexpected datatype for TIME vector.");

    DataSpace dataspace = dataset.getSpace();
    hsize_t dims[2];
    dataspace.getSimpleExtentDims(dims, NULL);

    // Check size of time vector is > 1, otherwise tricky errors
    // occurs further downstream in readWellStates()
    if ( (int)dims[0] < 2) {
        throw std::runtime_error("TIME vector has only one component! "
                                 "Check your simulation H5 output.");
    }

    std::vector<double> vector;
    vector.resize(dims[0]);
    dataset.read(vector.data(), PredType::NATIVE_DOUBLE);
    times_ = vector;
}

void Hdf5SummaryReader::readWellStates(std::string file_path) {
    // Read the file
    H5File file(file_path, H5F_ACC_RDONLY);
    Group group = Group(file.openGroup(GROUP_NAME_FLOW_TRANSPORT));
    DataSet dataset = DataSet(group.openDataSet(DATASET_NAME_WELL_STATES));

    DataSpace dataspace = dataset.getSpace();
    hsize_t dims[2];
    dataspace.getSimpleExtentDims(dims, NULL);

    nwells_ = (int)dims[0];
    ntimes_ = (int)dims[1];

    CompType ctype(sizeof(wstype_t));
    auto double_type = PredType::NATIVE_DOUBLE;
    auto int_type = PredType::NATIVE_INT;
    auto vdouble_type = VarLenType(&double_type);
    auto vint_type = VarLenType(&int_type);
    ctype.insertMember("vPressures", HOFFSET(wstype_t, vPressures_handle), vdouble_type);
    ctype.insertMember("vTemperatures", HOFFSET(wstype_t, vTemperatures_handle), vdouble_type);
    ctype.insertMember("vPhaseRates", HOFFSET(wstype_t, vPhaseRates_handle), vdouble_type);
    ctype.insertMember("vPhaseRatesAtSC", HOFFSET(wstype_t, vPhaseRatesAtSC_handle), vdouble_type);
    ctype.insertMember("vAverageDensity", HOFFSET(wstype_t, vAverageDensity_handle), vdouble_type);
    ctype.insertMember("vIntData", HOFFSET(wstype_t, vIntData_handle), vint_type);
    ctype.insertMember("vDoubleData", HOFFSET(wstype_t, vDoubleData_handle), vdouble_type);

    std::vector<wstype_t> data_vector;
    data_vector.resize(dims[0] * dims[1]);
    dataset.read(data_vector.data(), ctype);
    for (wstype_t &m : data_vector) {
        m.vPressures.assign(static_cast<double*>(m.vPressures_handle.p),
                            static_cast<double*>(m.vPressures_handle.p) + m.vPressures_handle.len);
        m.vTemperatures.assign(static_cast<double*>(m.vTemperatures_handle.p),
                               static_cast<double*>(m.vTemperatures_handle.p) + m.vTemperatures_handle.len);
        m.vPhaseRates.assign(static_cast<double*>(m.vPhaseRates_handle.p),
                             static_cast<double*>(m.vPhaseRates_handle.p) + m.vPhaseRates_handle.len);
        m.vPhaseRatesAtSC.assign(static_cast<double*>(m.vPhaseRatesAtSC_handle.p),
                                 static_cast<double*>(m.vPhaseRatesAtSC_handle.p) + m.vPhaseRatesAtSC_handle.len);
        m.vAverageDensity.assign(static_cast<double*>(m.vAverageDensity_handle.p),
                                 static_cast<double*>(m.vAverageDensity_handle.p) + m.vAverageDensity_handle.len);
        m.vIntData.assign(static_cast<int*>(m.vIntData_handle.p),
                          static_cast<int*>(m.vIntData_handle.p) + m.vIntData_handle.len);
        m.vDoubleData.assign(static_cast<double*>(m.vDoubleData_handle.p),
                             static_cast<double*>(m.vDoubleData_handle.p) + m.vDoubleData_handle.len);
    }
    parseWsVector(data_vector);

}

void Hdf5SummaryReader::parseWsVector(std::vector<wstype_t> &wsvec) {
    assert(wsvec.size() == nwells_*ntimes_);
    well_states_.resize(nwells_);
    for (int well = 0; well < nwells_; ++well) {
        well_states_[well] = parseWellState(wsvec, well);
    }
    nphases_ = well_states_[0].nphases;
}

Hdf5SummaryReader::well_data Hdf5SummaryReader::parseWellState(std::vector<wstype_t> &ws, int wnr) {
    int first = wnr*ntimes_;
    int last = first + ntimes_;
    int nperfs = (int)ws[first].vAverageDensity.size();
    auto state = Hdf5SummaryReader::well_data(ntimes_, nperfs);
    state.nphases = (int)ws[first].vPhaseRates.size() / nperfs;
    int t = 0;
    for (int i = first; i < last; ++i) { // Well data at each time step
        state.well_types[t] = ws[i].vIntData[0];
        state.phase_status[t] = ws[i].vIntData[1];
        state.well_controls[t] = ws[i].vIntData[2];
        state.bottom_hole_pressures[t] = ws[i].vPressures[0];
        if (state.nphases == 2) {
            state.water_rates_sc[t] = ws[i].vPhaseRatesAtSC[0];
            state.oil_rates_sc[t] = ws[i].vPhaseRatesAtSC[1];
            state.gas_rates_sc = std::vector<double>(ntimes_, 0.0);
        }
        else if (state.nphases == 3) {
            state.gas_rates_sc[t] = ws[i].vPhaseRatesAtSC[0];
            state.oil_rates_sc[t] = ws[i].vPhaseRatesAtSC[1];
            state.water_rates_sc[t] = ws[i].vPhaseRatesAtSC[2];
        } else throw std::runtime_error("Can only handle models with 2 or 3 phases, found " + boost::lexical_cast<std::string>(state.nphases));
        for (int p = 0; p < nperfs; ++p) { // Perforation data at each time step
            state.perforation_states[p] = perforation_data(ntimes_);
            state.perforation_states[p].pressures[t] = ws[i].vPressures[p+1];
            state.perforation_states[p].temperatures[t] = ws[i].vTemperatures[p];
            state.perforation_states[p].average_densities[t] = ws[i].vAverageDensity[p];
            if (state.nphases == 2) {
                state.perforation_states[p].water_rates[t] = ws[i].vPhaseRates[p*2 + 0];
                state.perforation_states[p].oil_rates[t] = ws[i].vPhaseRates[p*2 + 1];
            }
            else if (state.nphases == 3) {
                state.perforation_states[p].gas_rates[t] = ws[i].vPhaseRates[p*3 + 0];
                state.perforation_states[p].oil_rates[t] = ws[i].vPhaseRates[p*3 + 1];
                state.perforation_states[p].water_rates[t] = ws[i].vPhaseRates[p*3 + 2];
            } else throw std::runtime_error("Can only handle models with 2 or 3 phases.");
        }
        ++t;
    }
    return state;
}

int Hdf5SummaryReader::well_type(const int well_number) const {
    return well_states_[well_number].well_types[0];
}

bool Hdf5SummaryReader::is_injector(const int well_number) const {
    return well_states_[well_number].is_injector();
}

int Hdf5SummaryReader::number_of_perforations(const int well_number) const {
    return well_states_[well_number].nperfs;
}

int Hdf5SummaryReader::number_of_phases(const int well_number) const {
    return well_states_[well_number].nphases;
}

const std::vector<double> &Hdf5SummaryReader::bottom_hole_pressures(const int well_number) const {
    return well_states_[well_number].bottom_hole_pressures;
}

const std::vector<double> &Hdf5SummaryReader::oil_rates_sc(const int well_number) const {
    return well_states_[well_number].oil_rates_sc;
}

const std::vector<double> &Hdf5SummaryReader::water_rates_sc(const int well_number) const {
    return well_states_[well_number].water_rates_sc;
}

const std::vector<double> &Hdf5SummaryReader::gas_rates_sc(const int well_number) const {
    return well_states_[well_number].gas_rates_sc;
}

std::vector<double> Hdf5SummaryReader::calculate_cumulative(const std::vector<double> &rates) const {
    std::vector<double> cumulative(ntimes_, 0.0);
    for (int k = 1; k <ntimes_; ++k) {
        cumulative[k] = cumulative[k-1] + ((times_[k] - times_[k-1]) * (rates[k-1] + rates[k]) / 2.0);
    }
    return cumulative;
}

std::vector<double> Hdf5SummaryReader::cumulative_oil_production_sc(const int well_number) const {
    return calculate_cumulative(oil_rates_sc(well_number));
}

std::vector<double> Hdf5SummaryReader::cumulative_water_production_sc(const int well_number) const {
    return calculate_cumulative(water_rates_sc(well_number));
}

std::vector<double> Hdf5SummaryReader::cumulative_gas_production_sc(const int well_number) const {
    return calculate_cumulative(gas_rates_sc(well_number));
}

std::vector<double> Hdf5SummaryReader::field_oil_rates_sc() const {
    std::vector<double> sum(ntimes_, 0.0);
    for (int w = 0; w < nwells_; ++w) {
        if (is_injector(w)) continue; // Skip injectors
        auto well_rates = oil_rates_sc(w);
        for (int t = 0; t < ntimes_; ++t) {
            sum[t] = sum[t] + well_rates[t];
        }
    }
    return sum;
}

std::vector<double> Hdf5SummaryReader::field_water_rates_sc() const {
    std::vector<double> sum(ntimes_, 0.0);
    for (int w = 0; w < nwells_; ++w) {
        if (is_injector(w)) continue; // Skip injectors
        auto well_rates = water_rates_sc(w);
        for (int t = 0; t < ntimes_; ++t) {
            sum[t] = sum[t] + well_rates[t];
        }
    }
    return sum;
}

std::vector<double> Hdf5SummaryReader::field_gas_rates_sc() const {
    if (nphases_ == 2) throw std::runtime_error("Cannot get field gas rates for dead oil (two-phase) models.");
    std::vector<double> sum(ntimes_, 0.0);
    for (int w = 0; w < nwells_; ++w) {
        if (is_injector(w)) continue; // Skip injectors
        auto well_rates = gas_rates_sc(w);
        for (int t = 0; t < ntimes_; ++t) {
            sum[t] = sum[t] + well_rates[t];
        }
    }
    return sum;
}

int Hdf5SummaryReader::number_of_phases() const {
    return nphases_;
}

int Hdf5SummaryReader::cells_find_statuses(std::vector<int> &cells_all_vector_) {

    for (int i = 0; i < cells_all_vector_.size(); ++i) {
        if (cells_all_vector_[i] < 0){
            cells_inactive_.push_back(cells_all_vector_[i]);
            cells_inactive_idx_.push_back(i);
        }else{
            cells_active_.push_back(cells_all_vector_[i]);
            cells_active_idx_.push_back(i);
        }
    }

       cells_total_num_ = (int)cells_all_vector_.size();
      cells_num_active_ = (int)cells_active_.size();
    cells_num_inactive_ = (int)cells_inactive_.size();
}

std::vector<double> Hdf5SummaryReader::field_cumulative_oil_production_sc() const {
    std::vector<double> sum(ntimes_, 0.0);
    for (int w = 0; w < nwells_; ++w) {
        if (is_injector(w)) continue; // Skip injectors
        auto well_cumulatives = cumulative_oil_production_sc(w);
        for (int t = 0; t < ntimes_; ++t) {
            sum[t] = sum[t] + well_cumulatives[t];
        }
    }
    return sum;
}

std::vector<double> Hdf5SummaryReader::field_cumulative_water_production_sc() const {
    std::vector<double> sum(ntimes_, 0.0);
    for (int w = 0; w < nwells_; ++w) {
        if (is_injector(w)) continue; // Skip injectors
        auto well_cumulatives = cumulative_water_production_sc(w);
        for (int t = 0; t < ntimes_; ++t) {
            sum[t] = sum[t] + well_cumulatives[t];
        }
    }
    return sum;
}

std::vector<double> Hdf5SummaryReader::field_cumulative_gas_production_sc() const {
    std::vector<double> sum(ntimes_, 0.0);
    for (int w = 0; w < nwells_; ++w) {
        if (is_injector(w)) continue; // Skip injectors
        auto well_cumulatives = cumulative_gas_production_sc(w);
        for (int t = 0; t < ntimes_; ++t) {
            sum[t] = sum[t] + well_cumulatives[t];
        }
    }
    return sum;
}

std::vector<double> Hdf5SummaryReader::water_injection_rates_sc(const int well_number) const {
    if (!is_injector(well_number)) throw std::runtime_error("Attempted to get injecton rates for a production well.");
    std::vector<double> inj_rates(ntimes_, 0.0);
    auto rates = water_rates_sc(well_number);
    for (int t = 0; t < ntimes_; ++t) {
        inj_rates[t] = rates[t] * (-1.0);
    }
    return inj_rates;
}

std::vector<double> Hdf5SummaryReader::gas_injection_rates_sc(const int well_number) const {
    if (!is_injector(well_number)) throw std::runtime_error("Attempted to get injecton rates for a production well.");
    std::vector<double> inj_rates(ntimes_, 0.0);
    auto rates = gas_rates_sc(well_number);
    for (int t = 0; t < ntimes_; ++t) {
        inj_rates[t] = rates[t] * (-1.0);
    }
    return inj_rates;
}

std::vector<double> Hdf5SummaryReader::cumulative_water_injection_sc(const int well_number) const {
    return calculate_cumulative(water_injection_rates_sc(well_number));
}

std::vector<double> Hdf5SummaryReader::cumulative_gas_injection_sc(const int well_number) const {
    return calculate_cumulative(gas_injection_rates_sc(well_number));
}

std::vector<double> Hdf5SummaryReader::field_water_injection_rates_sc() const {
    std::vector<double> sum(ntimes_, 0.0);
    for (int w = 0; w < nwells_; ++w) {
        if (!is_injector(w)) continue; // Skip producers
        auto well_rates = water_injection_rates_sc(w);
        for (int t = 0; t < ntimes_; ++t) {
            sum[t] = sum[t] + well_rates[t];
        }
    }
    return sum;
}

std::vector<double> Hdf5SummaryReader::field_gas_injection_rates_sc() const {
    std::vector<double> sum(ntimes_, 0.0);
    for (int w = 0; w < nwells_; ++w) {
        if (!is_injector(w)) continue; // Skip producers
        auto well_rates = gas_injection_rates_sc(w);
        for (int t = 0; t < ntimes_; ++t) {
            sum[t] = sum[t] + well_rates[t];
        }
    }
    return sum;
}

std::vector<double> Hdf5SummaryReader::field_cumulative_water_injection_sc() const {
    std::vector<double> sum(ntimes_, 0.0);
    for (int w = 0; w < nwells_; ++w) {
        if (!is_injector(w)) continue; // Skip producers
        auto well_cumulatives = cumulative_water_injection_sc(w);
        for (int t = 0; t < ntimes_; ++t) {
            sum[t] = sum[t] + well_cumulatives[t];
        }
    }
    return sum;
}

std::vector<double> Hdf5SummaryReader::field_cumulative_gas_injection_sc() const {
    std::vector<double> sum(ntimes_, 0.0);
    for (int w = 0; w < nwells_; ++w) {
        if (!is_injector(w)) continue; // Skip producers
        auto well_cumulatives = cumulative_gas_injection_sc(w);
        for (int t = 0; t < ntimes_; ++t) {
            sum[t] = sum[t] + well_cumulatives[t];
        }
    }
    return sum;
}

Hdf5SummaryReader::well_data::well_data(int nt, int np) {
    nperfs = np;
    perforation_states.resize(np);
    well_types.resize(nt);
    phase_status.resize(nt);
    well_controls.resize(nt);
    bottom_hole_pressures.resize(nt);
    oil_rates_sc.resize(nt);
    water_rates_sc.resize(nt);
    gas_rates_sc.resize(nt);
}

Hdf5SummaryReader::perforation_data::perforation_data(int nt) {
    pressures.resize(nt);
    temperatures.resize(nt);
    average_densities.resize(nt);
    oil_rates.resize(nt);
    water_rates.resize(nt);
    gas_rates.resize(nt);
}
