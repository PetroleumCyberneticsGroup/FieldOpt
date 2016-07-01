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
    hsize_t dims_out[2];
    dataspace.getSimpleExtentDims(dims_out, NULL);

    std::vector<double> vector;
    vector.resize(dims_out[0]);
    dataset.read(vector.data(), PredType::NATIVE_DOUBLE);
    return vector;
}

void Hdf5SummaryReader::readWellStates(std::string file_path) {
    // Read the file
    H5File file(file_path, H5F_ACC_RDONLY);
    Group group = Group(file.openGroup(GROUP_NAME_FLOW_TRANSPORT));
    DataSet dataset = DataSet(group.openDataSet(DATASET_NAME_WELL_STATES));

    typedef struct wstype_t {
        std::vector<double> pressure;
        hvl_t pressure_handle;
        std::vector<double> temperature;
        hvl_t temperature_handle;
        std::vector<double> phaserate;
        hvl_t phaserate_handle;
        std::vector<double> phaseratesc;
        hvl_t phaseratesc_handle;
        std::vector<double> avgdensity;
        hvl_t avgdensity_handle;
        std::vector<int> intdata;
        hvl_t intdata_handle;
        std::vector<double> doubledata;
        hvl_t doubledata_handle;
    } wstype_t;
    CompType ctype(sizeof(wstype_t));
    auto double_type = PredType::NATIVE_DOUBLE;
    auto int_type = PredType::NATIVE_INT;
}

