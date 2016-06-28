#include "adgprsresults.h"
#include <iostream>

namespace Simulation { namespace Results {

AdgprsResults::AdgprsResults()
{}

double AdgprsResults::GetValue(int well_nr, Results::Property prop)
{
    throw std::runtime_error("Well properties are not available for ADGPRS results.");
}

double AdgprsResults::GetValue(int well_nr, Results::Property prop, int time_index)
{
    throw std::runtime_error("Well properties are not available for ADGPRS results.");
}

void AdgprsResults::ReadResults(QString file_path)
{
    if (file_path.split(".SIM.H5").length() == 1)
        file_path = file_path + ".SIM.H5"; // Append the suffix if it's not already there
    file_path_ = file_path;
    summary_reader_ = new AdgprsResultsReader::AdgprsResultsReader(file_path);
    setAvailable();
}

void AdgprsResults::ReadResults(QString file_path, QString build_dir)
{
    if (file_path.split(".SIM.H5").length() == 1)
        file_path = file_path + ".SIM.H5"; // Append the suffix if it's not already there
    file_path_ = file_path;
    build_dir_ = build_dir;
    summary_reader_ = new AdgprsResultsReader::AdgprsResultsReader(file_path, build_dir);
    setAvailable();
}

void AdgprsResults::DumpResults()
{
    delete summary_reader_;
    setUnavailable();
}

double AdgprsResults::GetValue(Results::Property prop)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ADGPRS");
    return summary_reader_->results()->GetFieldProperty(keys_[prop])->last();
}

double AdgprsResults::GetValue(Results::Property prop, QString well)
{
    throw std::runtime_error("Well properties are not available for ADGPRS results.");
}

double AdgprsResults::GetValue(Results::Property prop, int time_index)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ADGPRS");
    return summary_reader_->results()->GetFieldProperty(keys_[prop])->at(time_index);
}

double AdgprsResults::GetValue(Results::Property prop, QString well, int time_index)
{
    throw std::runtime_error("Well properties are not available for ADGPRS results.");
}

QVector<double> AdgprsResults::GetValueVector(Results::Property prop)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ADGPRS");
    return *summary_reader_->results()->GetFieldProperty(keys_[prop]);
}


}}
