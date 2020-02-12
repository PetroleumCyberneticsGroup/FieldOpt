#include "eclresults.h"
#include "ERTWrapper/ertwrapper_exceptions.h"
#include <QVector>
#include <boost/lexical_cast.hpp>
#include <Utilities/verbosity.h>
#include <Utilities/printer.hpp>

namespace Simulation {
namespace Results {

ECLResults::ECLResults()
    : Results()
{
    summary_reader_ = 0;
}

void ECLResults::ReadResults(QString file_path)
{
    if (VERB_SIM >= 2) {
        Printer::ext_info("Attempting to read results from" + file_path.toStdString(), "Simulation", "ECLResults");
    }
    file_path_ = file_path;
    if (summary_reader_ != 0) delete summary_reader_;
    try {
        summary_reader_ = new ERTWrapper::ECLSummary::ECLSummaryReader(file_path.toStdString());
    }
    catch (ERTWrapper::SummaryFileNotFoundAtPathException) {
        throw ResultFileNotFoundException(file_path.toLatin1().constData());
    }

    setAvailable();
}

void ECLResults::ReadResults(QString file_path, QString build_dir)
{

}

void ECLResults::DumpResults()
{
    if (summary_reader_ != 0) delete summary_reader_;
    summary_reader_ = 0;
    setUnavailable();
}

double ECLResults::GetValue(Results::Property prop)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    return GetValueVector(prop).back();
}

double ECLResults::GetValue(Results::Property prop, int time_index)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (time_index < 0 || time_index >= summary_reader_->time().size())
        throw std::runtime_error("The time index " + boost::lexical_cast<std::string>(time_index) + " is outside the range of the summary.");
    return GetValueVector(prop)[time_index];
}

double ECLResults::GetValue(Results::Property prop, QString well)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    return GetValueVector(prop, well).back();
}

double ECLResults::GetValue(Results::Property prop, QString well, int time_index)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (time_index < 0 || time_index >= summary_reader_->time().size())
        throw std::runtime_error("The time index " + boost::lexical_cast<std::string>(time_index) + " is outside the range of the summary.");
    return GetValueVector(prop, well)[time_index];
}

std::vector<double> ECLResults::GetValueVector(Results::Property prop)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    switch (prop) {
        case CumulativeOilProduction:   return summary_reader_->fopt();
        case CumulativeGasProduction:   return summary_reader_->fgpt();
        case CumulativeWaterProduction: return summary_reader_->fwpt();
        case CumulativeWaterInjection:  return summary_reader_->fwit();
        case CumulativeGasInjection:    return summary_reader_->fgit();
        case OilProductionRate:         return summary_reader_->fopr();
        case WaterProductionRate:       return summary_reader_->fwpr();
        case WaterInjectionRate:        return summary_reader_->fwir();
        case Time:                      return summary_reader_->time();
        default: throw std::runtime_error("In ECLResults: The requested property is not a field or misc property.");
    }
}

std::vector<double> ECLResults::GetValueVector(Results::Property prop, QString well_name) {
    if (!isAvailable()) throw ResultsNotAvailableException();
    switch (prop) {
        case CumulativeWellOilProduction:   return summary_reader_->wopt(well_name.toStdString());
        case CumulativeWellGasProduction:   return summary_reader_->wgpt(well_name.toStdString());
        case CumulativeWellWaterProduction: return summary_reader_->wwpt(well_name.toStdString());
        case CumulativeWellWaterInjection:  return summary_reader_->wwit(well_name.toStdString());
        case CumulativeWellGasInjection:    return summary_reader_->wgit(well_name.toStdString());
        case WellBottomHolePressure:        return summary_reader_->wbhp(well_name.toStdString());
        default: throw std::runtime_error("In ECLResults: The requested property is not a well property.");
    }
}

}}
