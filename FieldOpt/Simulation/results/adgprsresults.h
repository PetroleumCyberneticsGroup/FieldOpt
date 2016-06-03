#ifndef ADGPRSRESULTS_H
#define ADGPRSRESULTS_H

#include "results.h"
#include <QHash>
#include "AdgprsResultsReader/adgprs_results_reader.h"

namespace Simulation { namespace Results {

/*!
 * \brief AdgprsResults provides access to the results of an ADGPRS simulation.
 *
 * TODO: Support well properties. This is problematic because in the summary the wells only have a number that I can't reliably map to a name.
 */
class AdgprsResults : public Results
{
public:
    AdgprsResults();

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
