/******************************************************************************
 *
 * settings.cpp
 *
 * Created: 28.09.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#include "settings.h"
#include "settings_exceptions.h"

#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonArray>

namespace Utilities {
namespace Settings {

Settings::Settings(QString driver_path)
{
    driver_path_ = driver_path;
    readDriverFile();
}

Settings::Settings()
{
}

void Settings::readDriverFile()
{
    QFile *file = new QFile(driver_path_);
    if (!file->open(QIODevice::ReadOnly))
        throw DriverFileReadException("Unable to open the driver file");

    QByteArray data = file->readAll();

    QJsonDocument json = QJsonDocument::fromJson(data);
    if (json.isNull())
        throw DriverFileJsonParsingException("Unable to parse the input file to JSON.");

    if (!json.isObject())
        throw DriverFileFormatException("Driver file format incorrect. Must be a JSON object.");

    json_driver_ = new QJsonObject(json.object());

    readGlobalSection();
    readSimulatorSection();
    readOptimizerSection();
    readModelSection();

    file->close();
}

void Settings::readGlobalSection()
{
    try {
        QJsonObject global = json_driver_->value("Global").toObject();
        name_ = global["Name"].toString();
        output_directory_ = global["OutputDirectory"].toString();
        verbose_ = global["Verbose"].toBool();
    }
    catch (std::exception const &ex) {
        throw UnableToParseGlobalSectionException("Unable to parse driver file global section: " + std::string(ex.what()));
    }
}

void Settings::readSimulatorSection()
{
    // Simulator root
    try {
        QJsonObject json_simulator = json_driver_->value("Simulator").toObject();
        simulator_ = new Simulator(json_simulator);
    }
    catch (std::exception const &ex) {
        throw UnableToParseSimulatorSectionException("Unable to parse driver file simulator section: " + std::string(ex.what()));
    }
}

void Settings::readOptimizerSection()
{
    try {
        QJsonObject optimizer = json_driver_->value("Optimizer").toObject();
        optimizer_ = new Optimizer(optimizer);
    }
    catch (std::exception const &ex) {
        throw UnableToParseOptimizerSectionException("Unable to parse driver file optimizer section: " + std::string(ex.what()));
    }
}

void Settings::readModelSection()
{
    try {
        QJsonObject model = json_driver_->value("Model").toObject();
        model_ = new Model(model);
    }
    catch (std::exception const &ex) {
        throw UnableToParseModelSectionException("Unable to parse model section: " + std::string(ex.what()));
    }
}

}
}
