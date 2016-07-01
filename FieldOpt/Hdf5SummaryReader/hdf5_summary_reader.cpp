#include "hdf5_summary_reader.h"
#include <iostream>

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
    int rank = dataspace.getSimpleExtentNdims();
    std::cout << "Rank: " << rank << std::endl;
    hsize_t dims[2];
    int ndims = dataspace.getSimpleExtentDims(dims, NULL);
    std::cout << "Dims: " << ndims << " -- " << dims[0] << "x" << dims[1] << std::endl;

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
}

