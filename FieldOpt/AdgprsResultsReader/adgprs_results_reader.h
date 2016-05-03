#ifndef ADGPRSRESULTSREADER_H
#define ADGPRSRESULTSREADER_H

#include <QString>
#include "json_summary_reader.h"

namespace AdgprsResultsReader {

class AdgprsResultsReader
{
public:
    /*!
     * \brief AdgprsResultsReader
     * \param summary_path Path to a ADGPRS summary file in the HDF5 format, including the .SIM.H5 suffix.
     */
    AdgprsResultsReader(QString summary_path);

    ~AdgprsResultsReader();

    /*!
     * \brief results Get the object allowing access to the results.
     * \return
     */
    JsonSummaryReader *results() const { return json_reader_; }

private:
    QString hdf5_path_;
    QString json_path_;
    JsonSummaryReader *json_reader_;

    void convertHdfSummaryToJson();

};

}
#endif // ADGPRSRESULTSREADER_H
