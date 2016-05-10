#include "adgprsresults.h"
#include <iostream>

namespace Simulation { namespace Results {

AdgprsResults::AdgprsResults(Model::Model *model)
{
    for (int i = 0; i < model->wells()->size(); ++i) {
        well_numbers_[model->wells()->at(i)->name()] = i;
    }
}

double AdgprsResults::GetValue(int well_nr, Results::Property prop)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ADGPRS");
    if (summary_reader_->results()->IsInjector(well_nr))
        return 0.0;
    else
        return summary_reader_->results()->GetWellProperty(well_nr, keys_[prop])->last();
}

double AdgprsResults::GetValue(int well_nr, Results::Property prop, int time_index)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ADGPRS");
    if (summary_reader_->results()->IsInjector(well_nr))
        return 0.0;
    else
        return summary_reader_->results()->GetWellProperty(well_nr, keys_[prop])->at(time_index);
}

void AdgprsResults::ReadResults(QString file_path)
{
    if (file_path.split(".SIM.H5").length() == 1)
        file_path = file_path + ".SIM.H5"; // Append the suffix if it's not already there
    file_path_ = file_path;
    summary_reader_ = new AdgprsResultsReader::AdgprsResultsReader(file_path);
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
    if (!well_numbers_.contains(well)) throw std::runtime_error("Invalid well name. Cannot return results");
    return GetValue(well_numbers_[well], prop);
}

double AdgprsResults::GetValue(Results::Property prop, int time_index)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ADGPRS");
    return summary_reader_->results()->GetFieldProperty(keys_[prop])->at(time_index);
}

double AdgprsResults::GetValue(Results::Property prop, QString well, int time_index)
{
    if (!well_numbers_.contains(well)) throw std::runtime_error("Invalid well name. Cannot return results");
    return GetValue(well_numbers_[well], prop, time_index);
}

QVector<double> AdgprsResults::GetValueVector(Results::Property prop)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ADGPRS");
    return *summary_reader_->results()->GetFieldProperty(keys_[prop]);
}


}}
