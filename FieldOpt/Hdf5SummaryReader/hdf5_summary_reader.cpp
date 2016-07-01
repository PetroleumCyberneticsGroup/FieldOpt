#include "hdf5_summary_reader.h"
#include <iostream>
#include <assert.h>

using namespace H5;
Hdf5SummaryReader::Hdf5SummaryReader(const std::string file_path)
: GROUP_NAME_RESTART(("RESTART")), DATASET_NAME_TIMES("TIMES"),
  GROUP_NAME_FLOW_TRANSPORT("FLOW_TRANSPORT"), DATASET_NAME_WELL_STATES("WELL_STATES")
{
    times_ = readTimeVector(file_path);
    readWellStates(file_path);
}

std::vector<double> Hdf5SummaryReader::readTimeVector(std::string file_path) {
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

    std::vector<double> vector;
    vector.resize(dims[0]);
    dataset.read(vector.data(), PredType::NATIVE_DOUBLE);
    return vector;
}

void Hdf5SummaryReader::readWellStates(std::string file_path) {
    // Read the file
    H5File file(file_path, H5F_ACC_RDONLY);
    Group group = Group(file.openGroup(GROUP_NAME_FLOW_TRANSPORT));
    DataSet dataset = DataSet(group.openDataSet(DATASET_NAME_WELL_STATES));

    DataSpace dataspace = dataset.getSpace();
    hsize_t dims[2];
    dataspace.getSimpleExtentDims(dims, NULL);

    nwells_ = dims[0];
    ntimes_ = dims[1];

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
}

Hdf5SummaryReader::well_state Hdf5SummaryReader::parseWellState(std::vector<wstype_t> &ws, int wnr) {
    int first = wnr*ntimes_;
    int last = first + ntimes_;
    int nperfs = ws[first].vAverageDensity.size();
    auto state = Hdf5SummaryReader::well_state(ntimes_, nperfs);
    state.nphases = ws[0].vPhaseRates.size() / nperfs;
    int t = 0;
    for (int i = first; i < last; ++i) { // Well data at each time step
        state.well_types[t] = ws[i].vIntData[0];
        state.phase_status[t] = ws[i].vIntData[1];
        state.well_controls[t] = ws[i].vIntData[2];
        state.bottom_hole_pressures[t] = ws[i].vPressures[0];
        if (state.nphases == 2) {
            state.water_rates_sc[t] = ws[t].vPhaseRatesAtSC[0];
            state.oil_rates_sc[t] = ws[t].vPhaseRatesAtSC[1];
        }
        else if (state.nphases == 3) {
            state.gas_rates_sc[t] = ws[t].vPhaseRatesAtSC[0];
            state.oil_rates_sc[t] = ws[t].vPhaseRatesAtSC[1];
            state.water_rates_sc[t] = ws[t].vPhaseRatesAtSC[2];
        } else throw std::runtime_error("Can only handle models with 2 or 3 phases.");
        for (int p = 0; p < nperfs; ++p) { // Perforation data at each time step
            state.perforation_states[p] = perforation_state(ntimes_);
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

Hdf5SummaryReader::well_state::well_state(int nt, int np) {
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

Hdf5SummaryReader::perforation_state::perforation_state(int nt) {
    pressures.resize(nt);
    temperatures.resize(nt);
    average_densities.resize(nt);
    oil_rates.resize(nt);
    water_rates.resize(nt);
    gas_rates.resize(nt);
}
