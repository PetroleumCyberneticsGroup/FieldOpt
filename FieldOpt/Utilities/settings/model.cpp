/******************************************************************************
 *
 * model.cpp
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

#include "model.h"
#include "settings_exceptions.h"
#include <QJsonArray>

namespace Utilities {
namespace Settings {

Model::Model(QJsonObject json_model)
{
    try { // Reservoir
        QJsonObject json_reservoir = json_model["Reservoir"].toObject();
        readReservoir(json_reservoir);
    }
    catch (std::exception const &ex) {
        throw UnableToParseReservoirModelSectionException("Unable to parse reservoir model section: " + std::string(ex.what()));
    }

    // Wells
    try {
        QJsonArray json_wells = json_model["Wells"].toArray();
        wells_ = QList<Well>();
        for (int i = 0; i < json_wells.size(); ++i) {
            QJsonObject json_well = json_wells[i].toObject();
            wells_.append(readSingleWell(json_well));
        }
    }
    catch (std::exception const &ex) {
        throw UnableToParseWellsModelSectionException("Unable to parse wells model section: " + std::string(ex.what()));
    }
}

void Model::readReservoir(QJsonObject json_reservoir)
{
    // Reservoir grid source type
    QString type = json_reservoir["Type"].toString();
    if (QString::compare(type, "ECLIPSE") == 0)
        reservoir_.type = ReservoirGridSourceType::ECLIPSE;
    else throw UnableToParseReservoirModelSectionException("Grid source type " + type.toStdString() +  "not recognized.");

    // Reservoir grid path
    if (json_reservoir.contains("Path") && json_reservoir["Path"].toString().length() > 0)
        reservoir_.path = json_reservoir["Path"].toString();
    else throw UnableToParseReservoirModelSectionException("A reservoir path must be defined.");
}

Model::Well Model::readSingleWell(QJsonObject json_well)
{
    Well well;

    // Well name
    well.name = json_well["Name"].toString();

    // Well Type
    QString type = json_well["Type"].toString();
    if (QString::compare(type, "Producer") == 0)
        well.type = WellType::Producer;
    else if (QString::compare(type, "Injector") == 0)
        well.type = WellType::Injector;
    else throw UnableToParseWellsModelSectionException("Well type " + type.toStdString() + " not recognized for well " + well.name.toStdString());

    // Well definition type
    QString definition_type = json_well["DefinitionType"].toString();
    if (QString::compare(definition_type, "WellBlocks") == 0) {
        well.definition_type = WellDefinitionType::WellBlocks;
        well.well_blocks = QList<Well::IntegerCoordinate>();
        QJsonArray json_well_blocks = json_well["WellBlocks"].toArray();
        for (int i = 0; i < json_well_blocks.size(); ++i) {
            QJsonArray json_block = json_well_blocks[i].toArray();
            Well::IntegerCoordinate block;
            block.i = json_block[0].toInt();
            block.j = json_block[1].toInt();
            block.k = json_block[2].toInt();
            well.well_blocks.append(block);
        }
    }
    else if (QString::compare(definition_type, "WellSpline") == 0) {
        well.definition_type = WellDefinitionType::WellSpline;
        well.spline_points = QList<Well::RealCoordinate>();
        QJsonArray json_points = json_well["SplinePoints"].toArray();
        for (int i = 0; i < json_points.size(); ++i) {
            QJsonArray json_point = json_points[i].toArray();
            Well::RealCoordinate point;
            point.x = json_point[0].toDouble();
            point.y = json_point[1].toDouble();
            point.z = json_point[2].toDouble();
            well.spline_points.append(point);
        }
    }
    else throw UnableToParseWellsModelSectionException("Well definition type " + definition_type.toStdString() + " not recognized for well " + well.name.toStdString());

    // Well Control type
    QString control_type = json_well["Control"].toString();
    if (QString::compare(control_type, "BHP") == 0) {
        well.control = WellControlType::BHPControl;
        if (!json_well.contains("BHP"))
            throw UnableToParseWellsModelSectionException("BHP must be set when BHPControl is selected.");
        well.bhp = json_well["BHP"].toDouble();
    }
    else if (QString::compare(control_type, "Rate") == 0) {
        well.control = WellControlType::RateControl;
        if (!json_well.contains("Rate"))
            throw UnableToParseWellsModelSectionException("Rate must be set when RateControl is selected.");
        well.rate = json_well["Rate"].toDouble();
    }
    else throw UnableToParseWellsModelSectionException("Well control type " + control_type.toStdString() + " not recognized for well " + well.name.toStdString());

    // Heel
    if (!json_well.contains("Heel"))
        throw UnableToParseWellsModelSectionException("All wells must define a Heel.");
    QJsonArray json_heel = json_well["Heel"].toArray();
    well.heel.i = json_heel[0].toInt();
    well.heel.j = json_heel[1].toInt();
    well.heel.k = json_heel[2].toInt();

    // Variables
    if (json_well.contains("Variables")) {
        QJsonArray json_variables = json_well["Variables"].toArray();
        well.variables = QList<Well::Variable>();
        for (int i = 0; i < json_variables.size(); ++i) {
            QJsonObject json_variable = json_variables[i].toObject();
            well.variables.append(readSingleVariable(json_variable));
        }
    }
    return well;
}

Model::Well::Variable Model::readSingleVariable(QJsonObject json_variable)
{
    Well::Variable variable;
    QString type = json_variable["Type"].toString();
    if (QString::compare(type, "BHP") == 0) {
        variable.type = WellVariableType::BHP;
    }
    if (QString::compare(type, "Rate") == 0) {
        variable.type = WellVariableType::Rate;
    }
    else if (QString::compare(type, "SplinePoints") == 0) {
        variable.type = WellVariableType::SplinePoints;
        variable.variable_spline_point_indices = QList<int>();
        QJsonArray json_variable_spline_point_indices = json_variable["VariableSplinePointIndices"].toArray();
        for (int i = 0; i < json_variable_spline_point_indices.size(); ++i) {
            variable.variable_spline_point_indices.append(json_variable_spline_point_indices[i].toInt());
        }
    }

    variable.time_steps = QList<int>();
    QJsonArray json_time_steps = json_variable["TimeSteps"].toArray();
    for (int i = 0; i < json_time_steps.size(); ++i) {
        variable.time_steps.append(json_time_steps[i].toInt());
    }
    return variable;
}

}
}
