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

#include "Utilities/settings/model.h"
#include "settings_exceptions.h"
#include "Utilities/file_handling/filehandling.h"
#include <QJsonArray>

namespace Utilities {
namespace Settings {

Model::Model(QJsonObject json_model)
{
    // Reservoir
    try {
        QJsonObject json_reservoir = json_model["Reservoir"].toObject();
        readReservoir(json_reservoir);
    }
    catch (std::exception const &ex) {
        throw UnableToParseReservoirModelSectionException("Unable to parse reservoir model section: " + std::string(ex.what()));
    }

    // Control times
    if (!json_model.contains("ControlTimes") || !json_model["ControlTimes"].isArray())
        throw UnableToParseModelSectionException("The ControlTimes array must be defined with at leas one time for the model.");
    control_times_ = QList<int>();
    for (int i = 0; i < json_model["ControlTimes"].toArray().size(); ++i) {
        control_times_.append(json_model["ControlTimes"].toArray().at(i).toInt());
    }

    // Wells
    next_completion_id = 0;
    next_well_block_id = 0;
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
    if (json_reservoir.contains("Path") && json_reservoir["Path"].toString().length() > 0) {
        reservoir_.path = json_reservoir["Path"].toString();
        if (!::Utilities::FileHandling::FileExists(reservoir_.path))
            throw FileNotFoundException(reservoir_.path.toStdString());
    } else reservoir_.path = "";
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
        well.well_blocks = QList<WellBlock>();
        QJsonArray json_well_blocks = json_well["WellBlocks"].toArray();
        for (int i = 0; i < json_well_blocks.size(); ++i) {
            well.well_blocks.append(WellBlock(IntegerCoordinate(json_well_blocks[i].toArray()), next_well_block_id++));
        }
    }
    else if (QString::compare(definition_type, "WellSpline") == 0) {
        well.definition_type = WellDefinitionType::WellSpline;
        well.spline_points = QList<RealCoordinate>();
        QJsonArray json_points = json_well["SplinePoints"].toArray();
        for (int i = 0; i < json_points.size(); ++i) {
            well.spline_points.append(RealCoordinate(json_points[i].toArray()));
        }
    }
    else throw UnableToParseWellsModelSectionException("Well definition type " + definition_type.toStdString() + " not recognized for well " + well.name.toStdString());

    // Wellbore radius
    if (!json_well.contains("WellboreRadius"))
        throw UnableToParseWellsModelSectionException("The wellbore radius must be defined for all wells.");
    well.wellbore_radius = json_well["WellboreRadius"].toDouble();

    // Direction of penetration
    if (json_well.contains("Direction")) { // Direction must be specified for horizontal wells
        if (QString::compare("X", json_well["Direction"].toString()) == 0) well.direction = Direction::X;
        if (QString::compare("Y", json_well["Direction"].toString()) == 0) well.direction = Direction::Y;
        if (QString::compare("Z", json_well["Direction"].toString()) == 0) well.direction = Direction::Z;
    }

    // Controls
    QJsonArray json_controls = json_well["Controls"].toArray();
    well.controls = QList<ControlEntry>();
    for (int i = 0; i < json_controls.size(); ++i) {
        ControlEntry control;

        if (!controlTimeIsDeclared(json_controls.at(i).toObject()["TimeStep"].toInt()))
            throw UnableToParseWellsModelSectionException("All time steps must be declared in the ControlTimes array. Inconsistency detected in Controls declaration.");
        else control.time_step = json_controls.at(i).toObject()["TimeStep"].toInt();

        // State (Open or shut)
        if (QString::compare("Open", json_controls.at(i).toObject()["State"].toString()) == 0)
            control.state = WellState::WellOpen;
        else
            control.state = WellState::WellShut;

        // Control mode
        if (QString::compare("BHP", json_controls.at(i).toObject()["Mode"].toString()) == 0) {
            control.control_mode = ControlMode::BHPControl;
            control.bhp = json_controls.at(i).toObject()["BHP"].toDouble();
        }
        else if (QString::compare("Rate", json_controls.at(i).toObject()["Mode"].toString()) == 0) {
            control.control_mode = ControlMode::RateControl;
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

    // Prefered Phase
    if (QString::compare("Oil", json_well["PreferedPhase"].toString()) == 0)
        well.prefered_phase = PreferedPhase::Oil;
    else if (QString::compare("Water", json_well["PreferedPhase"].toString()) == 0)
        well.prefered_phase = PreferedPhase::Water;
    else if (QString::compare("Gas", json_well["PreferedPhase"].toString()) == 0)
        well.prefered_phase = PreferedPhase::Gas;
    else if (QString::compare("Liquid", json_well["PreferedPhase"].toString()) == 0)
        well.prefered_phase = PreferedPhase::Liquid;

    // Heel
    if (!json_well.contains("Heel"))
        throw UnableToParseWellsModelSectionException("All wells must define a Heel.");
    QJsonArray json_heel = json_well["Heel"].toArray();
    well.heel.i = json_heel[0].toInt();
    well.heel.j = json_heel[1].toInt();
    if (json_heel.size() == 3) // The k-index does not have to be specified. It is defaulted to 1 if it is not.
        well.heel.k = json_heel[2].toInt();
    else
        well.heel.k = 1;

    // Completions
    well.completions = QList<Completion>();
    if (json_well.contains("Completions")) {
        if (!json_well["Completions"].isArray())
            throw UnableToParseWellsModelSectionException("Completions must be provided as an array.");
        for (int i = 0; i < json_well["Completions"].toArray().size(); ++i) {
            QJsonObject json_completion = json_well["Completions"].toArray().at(i).toObject();
            Completion completion = Completion(next_completion_id++);
            QString completion_type = json_completion["Type"].toString();
            if (QString::compare(completion_type, "Perforation") == 0) {
                completion.type = WellCompletionType::Perforation;
                completion.transmissibility_factor = json_completion["TransmissibilityFactor"].toDouble();
                completion.well_block = getBlockAtPosition(well, json_completion["WellBlock"].toArray());
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
            well.variables.append(readSingleVariable(json_variable, well));
        }
    }
    return well;
}

Model::Well::Variable Model::readSingleVariable(QJsonObject json_variable, Well well)
{
    Well::Variable variable;

    if (!json_variable.contains("Name") || json_variable["Name"].toString().size() < 1 || variableNameExists(json_variable["Name"].toString()))
        throw UnableToParseWellsModelSectionException("All variables must specify a unique name.");
    variable.name = json_variable["Name"].toString();

    QString type = json_variable["Type"].toString();
    if (QString::compare(type, "BHP") == 0) {
        variable.type = WellVariableType::BHP;
    }
    else if (QString::compare(type, "OpenShut") == 0) {
        variable.type = WellVariableType::OpenShut;
    }
    else if (QString::compare(type, "Rate") == 0) {
        variable.type = WellVariableType::Rate;
    }
    else if (QString::compare(type, "Transmissibility") == 0) {
        variable.type = WellVariableType::Transmissibility;
        variable.blocks = QList<WellBlock>();
        if (!json_variable.contains("Blocks"))
            throw UnableToParseWellsModelSectionException("Transmissibility type variables must specify the Blocks property.");
        if (json_variable["Blocks"].isString() && QString::compare(json_variable["Blocks"].toString(), "WELL") == 0) { // Apply to all perforated well blocks
            for (int i = 0; i < well.completions.size(); ++i) {
                if (well.completions[i].type == WellCompletionType::Perforation)
                    variable.blocks.append(well.completions[i].well_block);
            }
        }
        else if (json_variable["Blocks"].isArray() && json_variable["Blocks"].toArray().first().isArray()) { // Parsing block list
            for (int i = 0; i < json_variable["Blocks"].toArray().size(); ++i) {
                variable.blocks.append(getBlockAtPosition(well, json_variable["Blocks"].toArray()[i].toArray()));
            }
        }
        else
            throw UnableToParseWellsModelSectionException("Unable to parse transmissibility variable Blocks property.");
    }
    else if (QString::compare(type, "WellBlockPosition") == 0) {
        variable.type = WellVariableType::WellBlockPosition;
        variable.blocks = QList<WellBlock>();
        if (!json_variable.contains("Blocks"))
            throw UnableToParseWellsModelSectionException("WellBlockPosition type variables must specify the Blocks property.");
        if (json_variable["Blocks"].isString() && QString::compare(json_variable["Blocks"].toString(), "WELL") == 0) { // Apply add all well blocks
            for (int i = 0; i < well.well_blocks.size(); ++i) {
                variable.blocks.append(well.well_blocks[i]);
            }
        }
        else if (json_variable["Blocks"].isArray() && json_variable["Blocks"].toArray().first().isArray()) { // Parsing block list
            for (int i = 0; i < json_variable["Blocks"].toArray().size(); ++i) {
                variable.blocks.append(getBlockAtPosition(well, json_variable["Blocks"].toArray()[i].toArray()));
            }
        }
        else
            throw UnableToParseWellsModelSectionException("Unable to parse WellBlockPosition variable Blocks property.");
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
        if (!controlTimeIsDeclared(json_time_steps[i].toInt()))
            throw UnableToParseWellsModelSectionException("All time steps must be declared in the ControlTimes array. Inconsistency detected in variable declaration.");
        variable.time_steps.append(json_time_steps[i].toInt());
    }
    if (!well.hasControlsCorrespondingToVariable(variable))
        throw UnableToParseWellsModelSectionException("A control of the correct type must be declared for all time steps in a control-type variable.");
    return variable;
}

bool Model::wellContainsBlock(Model::Well well, Model::IntegerCoordinate block)
{
    for (int i = 0; i < well.well_blocks.size(); ++i) {
        if (well.well_blocks[i].position.Equals(&block))
            return true;
    }
    return false;
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

bool Model::controlTimeIsDeclared(int time) const
{
    return control_times_.contains(time);
}

Model::WellBlock Model::getBlockAtPosition(Model::Well well, QJsonArray array)
{
    for (int i = 0; i < well.well_blocks.size(); ++i) {
        IntegerCoordinate pos = IntegerCoordinate(array);
        if (well.well_blocks[i].position.Equals(&pos))
            return well.well_blocks[i];
    }
    throw WellBlockNotFoundException("No well block found.");
}

bool Model::Well::hasControlsCorrespondingToVariable(Model::Well::Variable var)
{
    if (var.type != WellVariableType::BHP && var.type != WellVariableType::Rate)
        return true; // This check is only for rate/bhp control variables

    // Iterate over controls in well
    for (int control_entry = 0; control_entry < controls.size(); ++control_entry) {
        // If the variable and control entries match for either pressure or rate control ...
        if ((var.type == WellVariableType::BHP && controls[control_entry].control_mode == ControlMode::BHPControl)
                || (var.type == WellVariableType::Rate && controls[control_entry].control_mode == ControlMode::RateControl)) {
            // Iterate over time steps in variable
            for (int time_step = 0; time_step < var.time_steps.size(); ++time_step) {
                // If a time step in the variable matches the time step for the control
                if (controls[control_entry].time_step == var.time_steps[time_step])
                    return true;
            }
        }
    }
    return false;
}

}
}
