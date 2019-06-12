/******************************************************************************
   Created by einar on 6/11/19.
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "Simulation/results/json_results.h"
#include "Utilities/filehandling.hpp"
#include "QFile"
#include "QByteArray"
#include "QJsonDocument"
#include "QJsonObject"
#include "QJsonArray"
#include "Utilities/printer.hpp"
#include "Utilities/verbosity.h"

namespace Simulation {
namespace Results {

JsonResults::JsonResults(std::string file_path) {
    if (VERB_SIM >= 2) Printer::ext_info("Reading JSON results from " + file_path, "Simulation", "JsonResults");
    assert(Utilities::FileHandling::FileExists(file_path));
    if (VERB_SIM >= 2) Printer::info("JSON results file exists.");
    QFile *file = new QFile(QString::fromStdString(file_path));
    if (!file->open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Unable to open the JSON results file");
    }
    if (VERB_SIM >= 2) Printer::info("JSON results file successfully opened.");
    QByteArray data = file->readAll();
    QJsonDocument json = QJsonDocument::fromJson(data);

    if (json.isNull())
        throw std::runtime_error("Unable to parse the JSON results file.");

    if (!json.isObject())
        throw std::runtime_error("JSON results file format incorrect.");

    QJsonObject json_results = QJsonObject(json.object());
    if (VERB_SIM >= 2) Printer::info("JSON results file successfully read.");

    if (!json_results.contains("Components") || !json_results["Components"].isArray()) {
        throw std::runtime_error("The JSON results file must contain an array named Components.");
    }

    if (VERB_SIM >= 2) Printer::info("Parsing JSON results file contents.");
    for (auto comp : json_results["Components"].toArray()) {
        QJsonObject component = comp.toObject();
        if (component["Type"] == "Single") {
            singles_[component["Name"].toString().toStdString()] = component["Value"].toDouble();
        }
        else if (component["Type"] == "Monthly") {
            monthlies_[component["Name"].toString().toStdString()] = std::vector<double>();
            for (auto value : component["Values"].toArray()) {
                monthlies_[component["Name"].toString().toStdString()].push_back(value.toDouble());
            }
        }
        else if (component["Type"] == "Yearly") {
            yearlies_[component["Name"].toString().toStdString()] = std::vector<double>();
            for (auto value : component["Values"].toArray()) {
                yearlies_[component["Name"].toString().toStdString()].push_back(value.toDouble());
            }
        }
        else {
            throw std::runtime_error("Only parsing of Single, Monthly and Yearly type results have been implemented.");
        }
    }
    file->close();
    if (VERB_SIM >= 2) Printer::ext_info("Done reading additional JSON results.", "Simulation", "JsonResults");
}

double JsonResults::GetSingleValue(std::string name) {
    return singles_[name];
}
std::vector<double> JsonResults::GetMonthlyValues(std::string name) {
    return monthlies_[name];
}
std::vector<double> JsonResults::GetYearlyValues(std::string name) {
    return yearlies_[name];
}

}
}
