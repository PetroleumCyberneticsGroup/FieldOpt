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

#include <iostream>
#include "Utilities/settings/model.h"
#include "settings_exceptions.h"
#include "Utilities/file_handling/filehandling.h"

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

            // Well group
            if (json_well.contains("Group"))
                well.group = json_well["Group"].toString();
            else well.group = "";

            if (QString::compare(type, "Producer") == 0)
                well.type = WellType::Producer;
            else if (QString::compare(type, "Injector") == 0)
                well.type = WellType::Injector;
            else throw UnableToParseWellsModelSectionException("Well type " + type.toStdString() + " not recognized for well " + well.name.toStdString());

            // Well definition type
            QString definition_type = json_well["DefinitionType"].toString();
            if (QString::compare(definition_type, "WellBlocks") == 0) {
                well.definition_type = WellDefinitionType::WellBlocks;
                well.well_blocks = QList<Well::WellBlock>();
                QJsonArray json_well_blocks = json_well["WellBlocks"].toArray();
                for (int i = 0; i < json_well_blocks.size(); ++i) {
                    QJsonObject json_well_block_i = json_well_blocks[i].toObject();
                    Well::WellBlock block;
                    block.i = json_well_block_i["i"].toDouble();
                    block.j = json_well_block_i["j"].toDouble();
                    block.k = json_well_block_i["k"].toDouble();
                    if (json_well_block_i.contains("IsVariable") && json_well_block_i["IsVariable"].toBool() == true)
                        block.is_variable = true;
                    else block.is_variable = false;

                    if (json_well_block_i.contains("Completion")) {
                        QJsonObject json_well_block_i_completion = json_well_block_i["Completion"].toObject();
                        block.completion.type = WellCompletionType::Perforation;
                        if (json_well_block_i_completion.contains("TransmissibilityFactor"))
                            block.completion.transmissibility_factor = json_well_block_i_completion["TransmissibilityFactor"].toDouble();
                        if (json_well_block_i_completion.contains("IsVariable"))
                            block.completion.is_variable = json_well_block_i_completion["IsVariable"].toBool();
                        else
                            block.completion.is_variable = false;
                        block.completion.name = "Transmissibility#" + well.name + "#" + QString::number(i);
                        block.has_completion = true;
                    }
                    else
                        block.has_completion = false;
                    block.name = "WellBlock#" + well.name + "#" + QString::number(i);
                    well.well_blocks.append(block);
                }
            }
            else if (QString::compare(definition_type, "WellSpline") == 0) {
                well.definition_type = WellDefinitionType::WellSpline;
                QJsonObject json_points = json_well["SplinePoints"].toObject();
                if (!json_points.contains("Heel") || !json_points.contains("Toe"))
                    throw UnableToParseWellsModelSectionException("Both Heel and Toe must be defined for spline-type wells.");

                QJsonObject json_heel = json_points["Heel"].toObject();
                QJsonObject json_toe = json_points["Toe"].toObject();
                well.spline_heel.x = json_heel["x"].toDouble();
                well.spline_heel.y = json_heel["y"].toDouble();
                well.spline_heel.z = json_heel["z"].toDouble();
                well.spline_toe.x = json_toe["x"].toDouble();
                well.spline_toe.y = json_toe["y"].toDouble();
                well.spline_toe.z = json_toe["z"].toDouble();
                if (json_heel.contains("IsVariable") && json_heel["IsVariable"].toBool())
                    well.spline_heel.is_variable = true;
                if (json_toe.contains("IsVariable") && json_toe["IsVariable"].toBool())
                    well.spline_toe.is_variable = true;
                well.spline_heel.name = "SplinePoint#" + well.name + "#heel";
                well.spline_toe.name = "SplinePoint#" + well.name + "#toe";
            }
            else throw UnableToParseWellsModelSectionException("Well definition type " + definition_type.toStdString() + " not recognized for well " + well.name.toStdString());

            // Wellbore radius
            if (!json_well.contains("WellboreRadius"))
                throw UnableToParseWellsModelSectionException("The wellbore radius must be defined for all wells.");
            well.wellbore_radius = json_well["WellboreRadius"].toDouble();

            // Direction of penetration
            if (json_well.contains("Direction")) { // Direction must be specified for horizontal wells
                if (well.definition_type == WellDefinitionType::WellSpline)
                    throw std::runtime_error("Direction should not be specified for spline-defined wells");
                if (QString::compare("X", json_well["Direction"].toString()) == 0) well.direction = Direction::X;
                if (QString::compare("Y", json_well["Direction"].toString()) == 0) well.direction = Direction::Y;
                if (QString::compare("Z", json_well["Direction"].toString()) == 0) well.direction = Direction::Z;
            }

            // Controls
            QJsonArray json_controls = json_well["Controls"].toArray();
            well.controls = QList<Well::ControlEntry>();
            for (int i = 0; i < json_controls.size(); ++i) {
                Well::ControlEntry control;

                if (!controlTimeIsDeclared(json_controls.at(i).toObject()["TimeStep"].toInt()))
                    throw UnableToParseWellsModelSectionException("All time steps must be declared in the ControlTimes array. Inconsistency detected in Controls declaration.");
                else control.time_step = json_controls.at(i).toObject()["TimeStep"].toInt();

                // State (Open or shut)
                if (QString::compare("Shut", json_controls.at(i).toObject()["State"].toString()) == 0)
                    control.state = WellState::WellShut;
                else
                    control.state = WellState::WellOpen;


                // Control mode
                if (QString::compare("BHP", json_controls.at(i).toObject()["Mode"].toString()) == 0) {
                    control.control_mode = ControlMode::BHPControl;
                    control.bhp = json_controls.at(i).toObject()["BHP"].toDouble();
                    control.name = "BHP#" + well.name + "#" + QString::number(control.time_step);
                }
                else if (QString::compare("Rate", json_controls.at(i).toObject()["Mode"].toString()) == 0) {
                    control.control_mode = ControlMode::RateControl;
                    control.rate = json_controls.at(i).toObject()["Rate"].toDouble();
                    control.name = "Rate#" + well.name + "#" + QString::number(control.time_step);
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
                if (json_controls[i].toObject()["IsVariable"].toBool())
                    control.is_variable = true;
                else
                    control.is_variable = false;
                well.controls.append(control);
            }

            // Preferred Phase
            if (QString::compare("Oil", json_well["PreferredPhase"].toString()) == 0)
                well.preferred_phase = PreferredPhase::Oil;
            else if (QString::compare("Water", json_well["PreferredPhase"].toString()) == 0)
                well.preferred_phase = PreferredPhase::Water;
            else if (QString::compare("Gas", json_well["PreferredPhase"].toString()) == 0)
                well.preferred_phase = PreferredPhase::Gas;
            else if (QString::compare("Liquid", json_well["PreferredPhase"].toString()) == 0)
                well.preferred_phase = PreferredPhase::Liquid;

            return well;
        }

        bool Model::controlTimeIsDeclared(int time) const
        {
            return control_times_.contains(time);
        }



    }
}
