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

    // Controls
    QJsonArray json_controls = json_well["Controls"].toArray();
    well.controls = QList<Well::Control>();
    for (int i = 0; i < json_controls.size(); ++i) {
        Well::Control control;

        control.time_step = json_controls.at(i).toObject()["TimeStep"].toInt();

        // State (Open or shut)
        if (QString::compare("Open", json_controls.at(i).toObject()["State"].toString()) == 0)
            control.state = WellState::WellOpen;
        else
            control.state = WellState::WellShut;

        // Control mode
        if (QString::compare("BHP", json_controls.at(i).toObject()["Mode"].toString()) == 0) {
            control.control_mode = ControlType::BHPControl;
            control.bhp = json_controls.at(i).toObject()["BHP"].toDouble();
        }
        else if (QString::compare("Rate", json_controls.at(i).toObject()["Mode"].toString()) == 0) {
            control.control_mode = ControlType::RateControl;
            control.rate = json_controls.at(i).toObject()["Rate"].toDouble();
        }
        else throw UnableToParseWellsModelSectionException("Well control type " + json_controls.at(i).toObject()["Mode"].toString().toStdString() + " not recognized for well " + well.name.toStdString());

        // Injection type
        if (well.type == WellType::Injector) {
            if (!json_controls.at(i).toObject().contains("Type"))
                throw UnableToParseWellsModelSectionException("Type (water/gas) must be specified for injector wells.");
            if (QString::compare("Water", json_controls.at(i).toObject()["Type"].toString()) == 0)
                control.injection_type = InjectionType::WaterInjection;
            else if (QString::compare("Gas", json_controls.at(i).toObject()["Type"].toString()) == 0)
                control.injection_type = InjectionType::GasInjection;
        }
        well.controls.append(control);
    }

    // Heel
    if (!json_well.contains("Heel"))
        throw UnableToParseWellsModelSectionException("All wells must define a Heel.");
    QJsonArray json_heel = json_well["Heel"].toArray();
    well.heel.i = json_heel[0].toInt();
    well.heel.j = json_heel[1].toInt();
    well.heel.k = json_heel[2].toInt();

    // Completions
    well.completions = QList<Well::Completion>();
    if (json_well.contains("Completions")) {
        if (!json_well["Completions"].isArray())
            throw UnableToParseWellsModelSectionException("Completions must be provided as an array.");
        for (int i = 0; i < json_well["Completions"].toArray().size(); ++i) {
            QJsonObject json_completion = json_well["Completions"].toArray().at(i).toObject();
            Well::Completion completion;
            QString completion_type = json_completion["Type"].toString();
            if (QString::compare(completion_type, "Perforation") == 0) {
                completion.type = WellCompletionType::Perforation;
                completion.transmissibility_factor = json_completion["TransmissibilityFactor"].toDouble();
                completion.well_block.i = json_completion["WellBlock"].toArray().at(0).toInt();
                completion.well_block.j = json_completion["WellBlock"].toArray().at(1).toInt();
                completion.well_block.k = json_completion["WellBlock"].toArray().at(2).toInt();
            }
            else throw UnableToParseWellsModelSectionException("Completion type not recognized.");
            well.completions.append(completion);
        }
    }

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

    if (!json_variable.contains("Name") || json_variable["Name"].toString().size() < 1 || variableNameExists(json_variable["Name"].toString()))
        throw UnableToParseWellsModelSectionException("All variables must specify a unique name.");
    variable.name = json_variable["Name"].toString();

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

bool Model::variableNameExists(QString varible_name) const
{
    for (int i = 0; i < wells_.size(); ++i) {
        for (int j = 0; j < wells_.at(i).variables.size(); ++j) {
            if (QString::compare(wells_.at(i).variables.at(j).name, varible_name) == 0)
                return true;
        }
    }
    return false;
}

}
}
