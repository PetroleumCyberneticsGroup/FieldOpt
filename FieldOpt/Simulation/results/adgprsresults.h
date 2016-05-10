#ifndef ADGPRSRESULTS_H
#define ADGPRSRESULTS_H

#include "results.h"
#include <QHash>
#include "AdgprsResultsReader/adgprs_results_reader.h"
#include "Model/model.h"

namespace Simulation { namespace Results {

class AdgprsResults : public Results
{
public:
    /*!
     * \brief AdgprsResults Uses the Model to create a mapping from well name to well number.
     *
     * Creating this mapping is necessary because the ADGPRS does not (unlike ECL) use well
     * numbers in it's summary.
     *
     * \todo Well properties do not support injectors. Field props do.
     *
     * \todo The results reported for wells are not reliable. The nr -> name mapping is wrong.
     */
    AdgprsResults(Model::Model *model);

    double GetValue(int well_nr, Property prop);
    double GetValue(int well_nr, Property prop, int time_index);

    // Results interface
public:
    void ReadResults(QString file_path);
    void DumpResults();
    double GetValue(Property prop);
    double GetValue(Property prop, QString well);
    double GetValue(Property prop, int time_index);
    double GetValue(Property prop, QString well, int time_index);
    QVector<double> GetValueVector(Property prop);

private:
    QString file_path_;
    AdgprsResultsReader::AdgprsResultsReader *summary_reader_;
    QHash<QString, int> well_numbers_; //!< A mapping from well name to well number. This is needed because the ADGPRS summary does not include well names.

    // Mappings to summary keys.
    QHash<Property, QString> keys_ {
        {CumulativeOilProduction, "FOPT"},
        {CumulativeGasProduction, "FGPT"},
        {CumulativeWaterProduction, "FWPT"},
        {Time, "TIME"}
    };
};

}}

#endif // ADGPRSRESULTS_H
