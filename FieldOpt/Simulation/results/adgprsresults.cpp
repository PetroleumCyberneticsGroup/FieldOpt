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
    if (file_path.split(".vars.h5").length() == 1)
        file_path = file_path + ".vars.h5"; // Append the suffix if it's not already there
    file_path_ = file_path;
    summary_reader_ = new Hdf5SummaryReader(file_path_.toStdString());
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
    switch(prop) {
        case CumulativeOilProduction : return summary_reader_->field_cumulative_oil_production_sc().back();
        case CumulativeGasProduction : return summary_reader_->field_cumulative_gas_production_sc().back();
        case CumulativeWaterProduction : return summary_reader_->field_cumulative_water_production_sc().back();
        case Time : return summary_reader_->times_steps().back();
        default : throw std::runtime_error("Property type not recognized by AdgprsResults::GetValue");
    }
}

double AdgprsResults::GetValue(Results::Property prop, QString well)
{
    throw std::runtime_error("Well properties are not available for ADGPRS results.");
}

double AdgprsResults::GetValue(Results::Property prop, int time_index)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    switch(prop) {
        case CumulativeOilProduction : return summary_reader_->field_cumulative_oil_production_sc()[time_index];
        case CumulativeGasProduction : return summary_reader_->field_cumulative_gas_production_sc()[time_index];
        case CumulativeWaterProduction : return summary_reader_->field_cumulative_water_production_sc()[time_index];
        case Time : return summary_reader_->times_steps()[time_index];
        default : throw std::runtime_error("Property type not recognized by AdgprsResults::GetValue");
    }
}

double AdgprsResults::GetValue(Results::Property prop, QString well, int time_index)
{
    throw std::runtime_error("Well properties are not available for ADGPRS results.");
}

std::vector<double> AdgprsResults::GetValueVector(Results::Property prop)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    switch(prop) {
        case CumulativeOilProduction : return summary_reader_->field_cumulative_oil_production_sc();
        case CumulativeGasProduction : return summary_reader_->field_cumulative_gas_production_sc();
        case CumulativeWaterProduction : return summary_reader_->field_cumulative_water_production_sc();
        case Time : return summary_reader_->times_steps();
        default : throw std::runtime_error("Property type not recognized by AdgprsResults::GetValue");
    }
}


}}
