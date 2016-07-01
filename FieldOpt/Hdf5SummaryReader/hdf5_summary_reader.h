#ifndef FIELDOPT_HDF5SUMMARYREADER_H
#define FIELDOPT_HDF5SUMMARYREADER_H

#include <vector>
#include <H5Cpp.h>

class Hdf5SummaryReader {
public:
    Hdf5SummaryReader(const std::string file_path);
    const std::vector<double> &get_times() const { return times_; }

private:
    const H5std_string GROUP_NAME_RESTART;
    const H5std_string DATASET_NAME_TIMES;
    const H5std_string GROUP_NAME_FLOW_TRANSPORT;
    const H5std_string DATASET_NAME_WELL_STATES;

    std::vector<double> readTimeVector(std::string file_path);
    void readWellStates(std::string file_path);

    std::vector<double> times_;
};


#endif //FIELDOPT_HDF5SUMMARYREADER_H
