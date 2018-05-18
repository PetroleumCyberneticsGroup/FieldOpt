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
#include <sstream>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include "model.h"
#include "settings_exceptions.h"
#include "Utilities/filehandling.hpp"
#include "deck_parser.h"

namespace Settings {

Model::Model(QJsonObject json_model, Paths &paths)
{
    // Reservoir
    if (!paths.IsSet(Paths::ENSEMBLE_FILE)) {
        try {
            QJsonObject json_reservoir = json_model["Reservoir"].toObject();
            readReservoir(json_reservoir, paths);
        }
        catch (std::exception const &ex) {
            throw UnableToParseReservoirModelSectionException(
                "Unable to parse reservoir model section: " + std::string(ex.what()));
        }
    }

    // Control times
    if (!json_model.contains("ControlTimes") || !json_model["ControlTimes"].isArray())
        throw UnableToParseModelSectionException("The ControlTimes array must be defined with at least one time for the model.");
    control_times_ = QList<int>();
    for (int i = 0; i < json_model["ControlTimes"].toArray().size(); ++i) {
        control_times_.append(json_model["ControlTimes"].toArray().at(i).toInt());
    }

    // Wells
    wells_ = QList<Well>();
    if (json_model.contains("Import")) {
        auto json_import = json_model["Import"].toObject();
        if (!paths.IsSet(Paths::SIM_DRIVER_FILE)) {
            throw std::runtime_error("SchedulePath must be specified (relative to DriverPath) to use the Import feature.");
        }
        std::cout << "Parsing schedule ..." << std::endl;
        deck_parser_ = new DeckParser(paths.GetPath(Paths::SIM_DRIVER_FILE));

        if (!json_import["Keywords"].toArray().contains(QJsonValue("AllWells"))) {
            throw std::runtime_error("Unable to import simulator schedule. Import Keywords array does not contain"
                                         "any recognized keywords.");
        }
        std::cout << "Importing wells ..." << std::endl;
        wells_.append(deck_parser_->GetWellData());
        std::cout << "Done importing wells." << std::endl;
        setImportedWellDefaults(json_import);
        parseImportedWellOverrides(json_model["Wells"].toArray());
        if (json_import.contains("SplineConversion")) {
            auto json_sconv = json_import["SplineConversion"].toObject();
            if (!json_sconv.contains("Wells")) {
                throw std::runtime_error("A list of wells must be specified when the SplineConversion is used.");
            }
            for (auto jwell : json_sconv["Wells"].toArray()) {
                QString wname = jwell.toString();
                int widx = -1;
                for (int i = 0; i < wells_.size(); ++i) {
                    if (QString::compare(wells_[i].name, wname) == 0) {
                        widx = i;
                        break;
                    }
                }
                if (widx == -1) {
                    throw std::runtime_error("Well " + wname.toStdString() +
                        " defined in SplineConversion well list not found.");
                }
                wells_[widx].convert_well_blocks_to_spline = true;
                wells_[widx].definition_type = WellDefinitionType::WellSpline;
                if (json_sconv.contains("SplinePoints") && json_sconv["SplinePoints"].toInt() >= 2) {
                    wells_[widx].n_spline_points = json_sconv["SplinePoints"].toInt();
                    std::cout << "Setting number of spline points for " << wname.toStdString()
                              << " to " << wells_[widx].n_spline_points << " for conversion." << std::endl;
                }
                else {
                    std::cout << "Defaulting number of spline points for conversion to 2." << std::endl;
                }
            }
        }
    }
    else {
        try {
            QJsonArray json_wells = json_model["Wells"].toArray();
            for (int i = 0; i < json_wells.size(); ++i) {
                QJsonObject json_well = json_wells[i].toObject();
                wells_.append(readSingleWell(json_well));
            }
        }
        catch (std::exception const &ex) {
            throw UnableToParseWellsModelSectionException(
                "Unable to parse wells model section: " + std::string(ex.what()));
        }
    }
}

void Model::readReservoir(QJsonObject json_reservoir, Paths &paths)
{
    // Reservoir grid source type
    QString type = json_reservoir["Type"].toString();
    if (QString::compare(type, "ECLIPSE") == 0)
        reservoir_.type = ReservoirGridSourceType::ECLIPSE;
    else throw UnableToParseReservoirModelSectionException("Grid source type " + type.toStdString() +  "not recognized.");

    // Reservoir grid path
    if (!paths.IsSet(Paths::GRID_FILE) && json_reservoir.contains("Path")) {
        paths.SetPath(Paths::GRID_FILE, json_reservoir["Path"].toString().toStdString());
    }
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
        else well.spline_heel.is_variable = false;
        if (json_toe.contains("IsVariable") && json_toe["IsVariable"].toBool())
            well.spline_toe.is_variable = true;
        else well.spline_toe.is_variable = false;
        if ((well.spline_heel.is_variable && well.spline_toe.is_variable)
            || (json_points.contains("IsVariable") && json_points["IsVariable"].toBool() == true)) {
            well.is_variable_spline = true;
        }
        well.spline_heel.name = "SplinePoint#" + well.name + "#heel";
        well.spline_toe.name = "SplinePoint#" + well.name + "#toe";

        well.spline_points.push_back(well.spline_heel);
        if (json_points.contains("AdditionalPoints")) {
            int interp_points = json_points["AdditionalPoints"].toInt();
            for (int p = 0; p < interp_points; ++p) {
                Well::SplinePoint point;
                point.x = well.spline_heel.x + (p+1) * (well.spline_toe.x - well.spline_heel.x) / (interp_points+1);
                point.y = well.spline_heel.y + (p+1) * (well.spline_toe.y - well.spline_heel.y) / (interp_points+1);
                point.z = well.spline_heel.z + (p+1) * (well.spline_toe.z - well.spline_heel.z) / (interp_points+1);
                point.name = "SplinePoint#" + well.name + "#P" + QString::number(p+1);
                if (well.is_variable_spline) {
                    point.is_variable = true;
                }
                well.spline_points.push_back(point);
            }
        }
        well.spline_points.push_back(well.spline_toe);
    }
    else if (QString::compare(definition_type, "PseudoContVertical2D") == 0) {
        QJsonObject json_position = json_well["Position"].toObject();
        well.definition_type = WellDefinitionType::PseudoContVertical2D;
        well.pseudo_cont_position.i = json_position["i"].toInt();
        well.pseudo_cont_position.j = json_position["j"].toInt();
        if (json_position.contains("IsVariable") && json_position["IsVariable"].toBool() == true) {
            well.pseudo_cont_position.is_variable = true;
        }
        else well.spline_heel.is_variable = false;
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

    // Segmentation
    if (json_well.contains("Segmentation")) {
        parseSegmentation(json_well["Segmentation"].toObject(), well);
    }

    return well;
}

bool Model::controlTimeIsDeclared(int time) const
{
    return control_times_.contains(time);
}

std::string Model::Well::ControlEntry::toString() {
    std::stringstream ce;
    ce << "ControlEntry - Timestep:  " << time_step << "\n";
    ce << "               State:     " << (state == WellState::WellOpen ? "Open" : "Shut") << "\n";
    ce << "               Mode:      " << (control_mode == ControlMode::RateControl ? "Rate" : "BHP") << "\n";
    ce << "               BHP:       " << boost::lexical_cast<std::string>(bhp) << "\n";
    ce << "               Rate:      " << boost::lexical_cast<std::string>(rate) << "\n";
    ce << "               Inj. type: " << (injection_type == InjectionType::WaterInjection ? "Water" : "Gas/UNKWN") << "\n";
    ce << "               Variable:  " << (is_variable ? "Yes" : "No") << "\n";
    return ce.str();
}

std::string Model::Well::toString() {
    std::stringstream ce;
    ce << "Well - Name:           " << name.toStdString() << "\n";
    ce << "       Type:           " << (type == WellType::Injector ? "Injector" : "Producer") << "\n";
    ce << "       Group:          " << group.toStdString() << "\n";
    ce << "       Radius:         " << wellbore_radius << "\n";
    ce << "       Direction:      " << direction << "\n";
    ce << "       Pref. phase:    " << preferred_phase << "\n";
    ce << "       Def. type:      " << definition_type << "\n";
    ce << "       N. well blocks: " << well_blocks.size() << "\n";
    ce << "       N. controls:    " << controls.size() << "\n";
    return ce.str();
}

bool Model::Well::ControlEntry::isDifferent(ControlEntry other) {
    if (state != other.state)
        return true;
    if (rate != other.rate)
        return true;
    if (bhp != other.bhp)
        return true;
    if (control_mode != other.control_mode)
        return true;
    return false; // Assume they're equal if none of the above hits.
}

void Model::setImportedWellDefaults(QJsonObject json_import) {
    if (
        !json_import.contains("ProducerDefaultBHP") ||
        (!json_import.contains("InjectorDefaultRate") &&
        !json_import.contains("InjectorDefaultBHP"))
        ) {
        throw std::runtime_error("When importing from schedule, you must provide both the "
                                     "InjectorDefaultRate or InjectorDefaultBHP and "
                                     "ProducerDefaultBHP properties in the Import object.");
    }

    for (int i = 0; i < wells_.size(); ++i) {
        wells_[i].controls = QList<Well::ControlEntry>{wells_[i].controls[0]}; // Remove all but first control
        wells_[i].controls[0].time_step = getClosestControlTime(wells_[i].controls[0].time_step);
        if (wells_[i].type == Injector) {
            if (json_import.contains("InjectorDefaultRate")) {
                wells_[i].controls[0].control_mode = RateControl;
                wells_[i].controls[0].rate = json_import["InjectorDefaultRate"].toDouble();
            }
            else if (json_import.contains("InjectorDefaultBHP")) {
                wells_[i].controls[0].control_mode = BHPControl;
                wells_[i].controls[0].bhp = json_import["InjectorDefaultBHP"].toDouble();
            }
        }
        else { // Producer
            wells_[i].controls[0].control_mode = BHPControl;
            wells_[i].controls[0].bhp = json_import["ProducerDefaultBHP"].toDouble();
        }
    }
}

void Model::parseImportedWellOverrides(QJsonArray json_wells) {
    for (auto w : json_wells) {

        auto well = w.toObject();
        auto json_control = well["ControlOverrides"].toObject();
        if (json_control.contains("IsVariable") && json_control["IsVariable"].toBool() == true) {
            for (int i = 0; i < wells_.size(); ++i) { // Find the correct well
                if (QString::compare(wells_[i].name, well["Name"].toString()) ==0){
                    auto control = wells_[i].controls[0];
                    control.is_variable = true;
                    auto name_root_lst = control.name.split("#");
                    name_root_lst.removeLast();
                    auto name_root = name_root_lst.join("#") + "#";
                    auto new_control_set = QList<Well::ControlEntry>();
                    for (auto time : json_control["VariableTimeSteps"].toArray()) {
                        int t = time.toInt();
                        if (!control_times_.contains(t)) {
                            throw std::runtime_error("All entries in VariableTimeSteps must match the overall"
                                                         "ControlTimes vector. " + std::to_string(t) + " does not match");
                        }
                        auto control_copy = control;
                        control_copy.time_step = t;
                        control_copy.name = name_root + QString::number(t);
                        new_control_set.append(control_copy);
                    }
                    wells_[i].controls = new_control_set;
                    break;
                }
                else {
                    if (i == wells_.size() - 1) { // Passed the final well without finding a match.
                        throw std::runtime_error("Unable to find matching well for ControlOverrides");
                    }
                }
            }
        }
    }

}

int Model::getClosestControlTime(int deck_time) {
    auto diff = std::vector<int>(control_times_.size());
    for (int i = 0; i < control_times_.size(); ++i) { // Compute control times
        diff[i] = std::abs(deck_time - control_times_[i]);
    }
    auto idx_of_min_element = std::distance(diff.begin(), std::min_element(diff.begin(), diff.end()));
    return control_times_[idx_of_min_element];
}

void Model::parseSegmentation(QJsonObject json_seg, Well &well) {
    well.use_segmented_model = true;
    parseSegmentTubing(json_seg, well);
    parseSegmentAnnulus(json_seg, well);
    parseSegmentPackers(json_seg, well);
    parseSegmentICDs(json_seg, well);
}
void Model::parseSegmentTubing(const QJsonObject &json_seg, Model::Well &well) const {
    if (json_seg.contains("Tubing")) {
        try {
            well.seg_tubing.diameter = json_seg["Tubing"].toObject()["Diameter"].toDouble();
            well.seg_tubing.roughness = json_seg["Tubing"].toObject()["Roughness"].toDouble();
        }
        catch ( ... ) {
            throw std::runtime_error("For Tubing, both Diameter and Roughness must be defined.");
        }
    }
    else {
        std::cout << "Tubing keyword not found in Segmentation. "
            "Defaulting Diameter to 0.1 and Roughness to 1.52E-5" << std::endl;
        well.seg_tubing.diameter = 0.1;
        well.seg_tubing.roughness = 1.52E-5;
    }
}

void Model::parseSegmentAnnulus(const QJsonObject &json_seg, Model::Well &well) const {
    if (json_seg.contains("Annulus")) {
        try {
            well.seg_annulus.diameter = json_seg["Annulus"].toObject()["Diameter"].toDouble();
            well.seg_annulus.roughness = json_seg["Annulus"].toObject()["Roughness"].toDouble();
        }
        catch ( ... ) {
            throw std::runtime_error("For Annulus, both Diameter and Roughness must be defined.");
        }
    }
    else {
        std::cout << "Annulus keyword not found in Segmentation. "
            "Defaulting Diameter to 0.04 and Roughness to 1.52E-5" << std::endl;
        well.seg_annulus.diameter = 0.04;
        well.seg_annulus.roughness = 1.52E-5;
    }
}

void Model::parseSegmentPackers(const QJsonObject &json_seg, Model::Well &well) const {
    if (json_seg.contains("AutoPackers")) {
        auto json_packer = json_seg["AutoPackers"].toObject();
        try {
            well.seg_n_auto_packers = json_packer["Count"].toInt();
        }
        catch ( ... ) {
            throw std::runtime_error("For AutoPackers, Count must be defined.");
        }
        if ( (json_packer.contains("VariablePlacement") && json_packer["VariablePlacement"].toBool() == true) ||
             (json_packer.contains("IsVariable") || json_packer["IsVariable"].toBool() == true) )
        {
            well.seg_auto_packer_params.variable_placement = true;
            well.seg_auto_packer_params.is_variable = true;
        }
    }
}

void Model::parseSegmentICDs(const QJsonObject &json_seg, Model::Well &well) const {
    if (json_seg.contains("AutoICDs")) {
        auto json_icd = json_seg["AutoICDs"].toObject();
        try {
            well.seg_n_auto_icds = json_icd["Count"].toInt();
            well.seg_auto_icd_params.diameter = json_icd["Diameter"].toDouble();
            well.seg_auto_icd_params.valve_size = json_icd["ValveSize"].toDouble();
            if (QString::compare(json_icd["Type"].toString(), "Valve") == 0) {
                well.seg_auto_icd_params.type = WellCompletionType::ICV;
            }
            else {
                std::cout << "ICD type not recognized. Defaulting to Valve (ICV)" << std::endl;
                well.seg_auto_icd_params.type = WellCompletionType::ICV;
            }
            if (json_icd.contains("Roughness")) {
                well.seg_auto_icd_params.roughness = json_icd["Roughness"].toDouble();
            }
            else {
                std::cout << "Roughness keyword not found in AutoICDs. " "Defaulting Roughness  1.52E-5" << std::endl;
                well.seg_annulus.roughness = 1.52E-5;
            }
        }
        catch ( ... ) {
            throw std::runtime_error("For AutoICDs, Diameter, ValveSize, Type and Count must be defined.");
        }
        if (json_icd.contains("IsVariable")) {
            throw std::runtime_error("The IsVariable keyword does not work for ICDs. "
                                         "Use VariablePlacment and/or VariableStrength instead.")
        }
        if (json_icd.contains("VariablePlacement") && json_icd["VariablePlacement"].toBool() == true) {
            well.seg_auto_icd_params.variable_placement = true;
        }
        if (json_icd.contains("VariableStrength") && json_icd["VariableStrength"].toBool() == true) {
            well.seg_auto_icd_params.variable_strength = true;
        }
        if (well.seg_auto_icd_params.variable_strength && well.seg_auto_icd_params.variable_placement) {
            well.seg_auto_icd_params.is_variable = true;
        }
    }
}

}

