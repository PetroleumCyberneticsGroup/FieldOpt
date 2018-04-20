/***********************************************************
 model.cpp

 Created: 28.09.2015 2015 by einar

 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version
 3 of the License, or (at your option) any later version.

 FieldOpt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the
 GNU General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include <iostream>
#include "model.h"
#include "settings_exceptions.h"
#include "Utilities/filehandling.hpp"

// ---------------------------------------------------------
using std::cout;
using std::endl;

// ---------------------------------------------------------
namespace Settings {

// =========================================================
Model::Model(QJsonObject json_model) {

  // -------------------------------------------------------------
  // Reservoir
  try {
    QJsonObject json_reservoir = json_model["Reservoir"].toObject();
    readReservoir(json_reservoir);
  }
  catch (std::exception const &ex) {
    throw UnableToParseReservoirModelSectionException(
        "Unable to parse reservoir model section: "
            + std::string(ex.what()));
  }

  // -------------------------------------------------------------
  // Check control times listing
  if (!json_model.contains("ControlTimes")
      || !json_model["ControlTimes"].isArray())
    throw UnableToParseModelSectionException(
        "The ControlTimes array must be defined "
            "with at least one time for the model.");

  // -------------------------------------------------------------
  // Append control times
  control_times_ = QList<double>();
  for (int i = 0; i < json_model["ControlTimes"].toArray().size(); ++i) {
    control_times_.append(
        json_model["ControlTimes"].toArray().at(i).toDouble());
  }

  // -------------------------------------------------------------
  // Loop through wells
  try {

    // -----------------------------------------------------------
    QJsonArray json_wells = json_model["Wells"].toArray();
    wells_ = QList<Well>();

    // -----------------------------------------------------------
    for (int i = 0; i < json_wells.size(); ++i) {
      QJsonObject json_well = json_wells[i].toObject();
      wells_.append(readSingleWell(json_well));
    }

  } catch (std::exception const &ex) {
    throw UnableToParseWellsModelSectionException(
        "Unable to parse wells model section: "
            + std::string(ex.what()));
  }

  // -------------------------------------------------------------
  // Drilling sequence mode
  QString drilling = json_model["Drilling"].toString();
  if (QString::compare(drilling, "Synchronous") == 0) {
    drilling_.mode = DrillingMode::Synchronous;
  } else if (QString::compare(drilling, "Sequential") == 0) {
    drilling_.mode = DrillingMode::Sequential;
  } else {
    drilling_.mode = DrillingMode::Synchronous;
  }

  // -------------------------------------------------------------
  // Main drilling order maps (pairs)
  for (int i = 0; i < wells_.size(); ++i) {

    // -----------------------------------------------------------
    std::pair<std::string, std::pair<int, int>>
        well_order(wells_[i].name.toStdString(),
                   wells_[i].drilling_order);
    drilling_.order.push_back(well_order);

    // -----------------------------------------------------------
    std::pair<std::string, double>
        well_time(wells_[i].name.toStdString(),
                  wells_[i].drilling_time);
    drilling_.time.push_back(well_time);

  }

  // -------------------------------------------------------------
  // Drilling sequence by group (<int, string>)
  for (int i = 0; i < drilling_.order.size(); ++i) {

    std::pair<int, string> p(drilling_.order[i].second.second,
                             drilling_.order[i].first);
    drilling_.seq_by_group_mp.emplace(drilling_.order[i].second.first, p);

  }


  // -------------------------------------------------------------
  // Loop by group
  decltype(drilling_.seq_by_group_mp.equal_range(int())) range;

  for (auto i = drilling_.seq_by_group_mp.begin();
    i != drilling_.seq_by_group_mp.end(); i = range.second) {

    range = drilling_.seq_by_group_mp.equal_range(i->first);

    // -----------------------------------------------------------
    vector<pair<int, string>> isv;
    for(auto d = range.first; d != range.second; ++d) {

      // Dbg
      std::cout << "Grp:" << d->first
                << " Order w/i grp: " << d->second.first
                << " Well:" << d->second.second << std::endl;

      pair<int, string> is(d->second.first, d->second.second);
      isv.push_back(is);

    }

    drilling_.seq_by_group_vec.push_back(isv);
  }

  // -------------------------------------------------------------
  multimap<int,string> mp;
  for (int j=0; j<drilling_.seq_by_group_vec.size(); j++) {

    for (int k=0; k<drilling_.seq_by_group_vec[j].size(); k++) {

      mp.emplace(drilling_.seq_by_group_vec[j][k].first,
                drilling_.seq_by_group_vec[j][k].second);

      // Dbg
      cout << "[ " << drilling_.seq_by_group_vec[j][k].first << " -- "
           << drilling_.seq_by_group_vec[j][k].second << " ]" << endl;
    }
  }

  // -------------------------------------------------------------
  decltype(mp.equal_range(int())) range2;
  for (auto i = mp.begin(); i != mp.end(); i = range2.second) {
    range2 = mp.equal_range(i->first);
    for(auto d = range2.first; d != range2.second; ++d) {
      // Dbg
      std::cout << "[ " << d->first
                << " ## " << d->second
                << " ]" << std::endl;
    }
  }


}

// =========================================================
void Model::readReservoir(QJsonObject json_reservoir) {

  // -------------------------------------------------------------
  // Reservoir grid source type
  QString type = json_reservoir["Type"].toString();
  if (QString::compare(type, "ECLIPSE") == 0)
    reservoir_.type = ReservoirGridSourceType::ECLIPSE;
  else throw UnableToParseReservoirModelSectionException(
        "Grid source type " + type.toStdString() +  "not recognized.");

  // -------------------------------------------------------------
  // Reservoir grid path
  if (json_reservoir.contains("Path")
      && json_reservoir["Path"].toString().length() > 0) {

    reservoir_.path = json_reservoir["Path"].toString();
    if (!::Utilities::FileHandling::FileExists(reservoir_.path))
      throw FileNotFoundException(reservoir_.path.toStdString());
  } else { reservoir_.path = ""; }

}

// =========================================================
Model::Well Model::readSingleWell(QJsonObject json_well) {

  // -------------------------------------------------------------
  Well well;

  // -------------------------------------------------------------
  // Well name
  well.name = json_well["Name"].toString();

  // -------------------------------------------------------------
  // Well Type
  QString type = json_well["Type"].toString();

  // -------------------------------------------------------------
  // Well group
  if (json_well.contains("Group"))
    well.group = json_well["Group"].toString();
  else well.group = "";

  // -------------------------------------------------------------
  if (QString::compare(type, "Producer") == 0)
    well.type = WellType::Producer;
  else if (QString::compare(type, "Injector") == 0)
    well.type = WellType::Injector;
  else {
    throw UnableToParseWellsModelSectionException(
        "Well type " + type.toStdString()
            + " not recognized for well "
            + well.name.toStdString());
  }

  // -------------------------------------------------------------
  // Get well definition type
  QString definition_type = json_well["DefinitionType"].toString();

  // -------------------------------------------------------------
  // Well definition type: WellBlocks
  if (QString::compare(definition_type, "WellBlocks") == 0) {

    // -----------------------------------------------------------
    well.definition_type = WellDefinitionType::WellBlocks;
    well.well_blocks = QList<Well::WellBlock>();
    QJsonArray json_well_blocks = json_well["WellBlocks"].toArray();

    // -----------------------------------------------------------
    for (int i = 0; i < json_well_blocks.size(); ++i) {

      // ---------------------------------------------------------
      // Make block and set corresponding IJK coords to it
      QJsonObject json_well_block_i = json_well_blocks[i].toObject();
      Well::WellBlock block;
      block.i = json_well_block_i["i"].toInt();
      block.j = json_well_block_i["j"].toInt();
      block.k = json_well_block_i["k"].toInt();

      // ---------------------------------------------------------
      // Check if block is set to variable
      if (json_well_block_i.contains("IsVariable")
          && json_well_block_i["IsVariable"].toBool() == true) {
        block.is_variable = true;
      } else {
        block.is_variable = false;
      }

      // ---------------------------------------------------------
      if (json_well_block_i.contains("Completion")) {

        // -------------------------------------------------------
        QJsonObject json_well_block_i_completion =
            json_well_block_i["Completion"].toObject();
        block.completion.type = WellCompletionType::Perforation;

        // -------------------------------------------------------
        if (json_well_block_i_completion.contains("TransmissibilityFactor"))
          block.completion.transmissibility_factor =
              json_well_block_i_completion["TransmissibilityFactor"].toDouble();

        // -------------------------------------------------------
        if (json_well_block_i_completion.contains("IsVariable"))
          block.completion.is_variable = json_well_block_i_completion["IsVariable"].toBool();
        else
          block.completion.is_variable = false;

        // -------------------------------------------------------
        block.completion.name =
            "Transmissibility#" + well.name + "#" + QString::number(i);
        block.has_completion = true;

      } else {
        block.has_completion = false;
      }

      // ---------------------------------------------------------
      block.name = "WellBlock#" + well.name + "#" + QString::number(i);
      well.well_blocks.append(block);
    }

    // -------------------------------------------------------------
    // Well definition type: WellSpline
  } else if (QString::compare(definition_type, "WellSpline") == 0) {

    well.definition_type = WellDefinitionType::WellSpline;
    QJsonObject json_points = json_well["SplinePoints"].toObject();

    // -------------------------------------------------------------
    if (!json_points.contains("Heel") || !json_points.contains("Toe")){
      throw UnableToParseWellsModelSectionException(
          "Both Heel and Toe must be defined for spline-type wells.");
    }

    // -------------------------------------------------------------
    QJsonObject json_heel = json_points["Heel"].toObject();
    QJsonObject json_toe = json_points["Toe"].toObject();

    // -------------------------------------------------------------
    well.spline_heel.x = json_heel["x"].toDouble();
    well.spline_heel.y = json_heel["y"].toDouble();
    well.spline_heel.z = json_heel["z"].toDouble();

    // -------------------------------------------------------------
    well.spline_toe.x = json_toe["x"].toDouble();
    well.spline_toe.y = json_toe["y"].toDouble();
    well.spline_toe.z = json_toe["z"].toDouble();

    // -------------------------------------------------------------
    if (json_heel.contains("IsVariable")
        && json_heel["IsVariable"].toBool()) {
      well.spline_heel.is_variable = true;

    } else well.spline_heel.is_variable = false;

    // -------------------------------------------------------------
    if (json_toe.contains("IsVariable")
        && json_toe["IsVariable"].toBool()) {
      well.spline_toe.is_variable = true;

    } else well.spline_toe.is_variable = false;

    // -------------------------------------------------------------
    well.spline_heel.name = "SplinePoint#" + well.name + "#heel";
    well.spline_toe.name = "SplinePoint#" + well.name + "#toe";

    // -------------------------------------------------------------
  } else if (QString::compare(definition_type,
                              "PseudoContVertical2D") == 0) {

    // -------------------------------------------------------------
    QJsonObject json_position = json_well["Position"].toObject();
    well.definition_type = WellDefinitionType::PseudoContVertical2D;
    well.pseudo_cont_position.i = json_position["i"].toInt();
    well.pseudo_cont_position.j = json_position["j"].toInt();

    // -------------------------------------------------------------
    if (json_position.contains("IsVariable")
        && json_position["IsVariable"].toBool() == true) {
      well.pseudo_cont_position.is_variable = true;
    }
    else well.spline_heel.is_variable = false;

  } else {
    // -------------------------------------------------------------
    throw UnableToParseWellsModelSectionException(
        "Well definition type " + definition_type.toStdString()
            + " not recognized for well " + well.name.toStdString());
  }

  // ---------------------------------------------------------
  // Wellbore radius
  if (!json_well.contains("WellboreRadius")) {
    throw UnableToParseWellsModelSectionException(
        "The wellbore radius must be defined for all wells.");
  }
  well.wellbore_radius = json_well["WellboreRadius"].toDouble();

  // ---------------------------------------------------------
  // Drilling time
  if (json_well.contains("DrillingTime")) {
    well.drilling_time = json_well["DrillingTime"].toDouble();
  }

  // ---------------------------------------------------------
  // Drilling sequence
  well.drilling_order = std::pair<int, int>(2,-1); // Default vals
  if (json_well.contains("DrillingSequence")) {

    well.drilling_order.first =
        json_well["DrillingSequence"].toArray().at(0).toInt();
    well.drilling_order.second =
        json_well["DrillingSequence"].toArray().at(1).toInt();

  }

  // ---------------------------------------------------------
  // Direction of penetration
  // Direction must be specified for horizontal wells
  if (json_well.contains("Direction")) {

    // -------------------------------------------------------------
    if (well.definition_type == WellDefinitionType::WellSpline) {
      throw std::runtime_error(
          "Direction should not be specified for spline-defined wells");
    }

    // -------------------------------------------------------------
    if (QString::compare("X", json_well["Direction"].toString()) == 0) {
      well.direction = Direction::X;
    }

    // -------------------------------------------------------------
    if (QString::compare("Y", json_well["Direction"].toString()) == 0) {
      well.direction = Direction::Y;
    }

    // -------------------------------------------------------------
    if (QString::compare("Z", json_well["Direction"].toString()) == 0) {
      well.direction = Direction::Z;
    }
  }

  // ---------------------------------------------------------
  // Controls
  QJsonArray json_controls = json_well["Controls"].toArray();
  well.controls = QList<Well::ControlEntry>();

  // ---------------------------------------------------------
  for (int i = 0; i < json_controls.size(); ++i) {
    Well::ControlEntry control;

    // -------------------------------------------------------------
    double ctime = json_controls.at(i).toObject()["TimeStep"].toDouble();
    if (!controlTimeIsDeclared(ctime)) {

      throw UnableToParseWellsModelSectionException(
          "All time steps must be declared in the ControlTimes array. "
              "Inconsistency detected in Controls declaration.");
    }
    else control.time_step = ctime;

    // -------------------------------------------------------------
    // State (Open or shut)
    if (QString::compare(
        "Shut", json_controls.at(i).toObject()["State"].toString()) == 0){
      control.state = WellState::WellShut;

    } else control.state = WellState::WellOpen;

    // -------------------------------------------------------------
    // Control mode
    if (QString::compare(
        "BHP", json_controls.at(i).toObject()["Mode"].toString()) == 0) {

      // -----------------------------------------------------------
      control.control_mode = ControlMode::BHPControl;
      control.bhp = json_controls.at(i).toObject()["BHP"].toDouble();
      control.name = "BHP#" + well.name + "#" + QString::number(control.time_step);

      // -----------------------------------------------------------
    } else if (QString::compare(
        "Rate", json_controls.at(i).toObject()["Mode"].toString()) == 0) {

      // -----------------------------------------------------------
      control.control_mode = ControlMode::RateControl;
      control.rate = json_controls.at(i).toObject()["Rate"].toDouble();
      control.name = "Rate#" + well.name + "#" + QString::number(control.time_step);

    } else {
      throw UnableToParseWellsModelSectionException(
          "Well control type " +
              json_controls.at(i).toObject()["Mode"].toString().toStdString()
              + " not recognized for well " + well.name.toStdString());
    }

    // -------------------------------------------------------------
    // Injection type
    if (well.type == WellType::Injector) {

      // -----------------------------------------------------------
      if (!json_controls.at(i).toObject().contains("Type")) {
        throw UnableToParseWellsModelSectionException(
            "Type (water/gas) must be specified for injector wells.");
      }

      // -----------------------------------------------------------
      if (QString::compare(
          "Water", json_controls.at(i).toObject()["Type"].toString()) == 0) {
        control.injection_type = InjectionType::WaterInjection;

      } else if (QString::compare(
          "Gas", json_controls.at(i).toObject()["Type"].toString()) == 0) {
        control.injection_type = InjectionType::GasInjection;
      }
    }

    // -------------------------------------------------------------
    if (json_controls[i].toObject()["IsVariable"].toBool()) {
      control.is_variable = true;
    } else {
      control.is_variable = false;
    }

    // -------------------------------------------------------------
    well.controls.append(control);
  }

  // ---------------------------------------------------------
  // Preferred Phase
  if (QString::compare(
      "Oil", json_well["PreferredPhase"].toString()) == 0) {
    well.preferred_phase = PreferredPhase::Oil;

  } else if (QString::compare(
      "Water", json_well["PreferredPhase"].toString()) == 0) {
    well.preferred_phase = PreferredPhase::Water;

  } else if (QString::compare(
      "Gas", json_well["PreferredPhase"].toString()) == 0) {
    well.preferred_phase = PreferredPhase::Gas;

  } else if (QString::compare(
      "Liquid", json_well["PreferredPhase"].toString()) == 0) {
    well.preferred_phase = PreferredPhase::Liquid;
  }

  // -------------------------------------------------------
  return well;
}

// =========================================================
bool Model::controlTimeIsDeclared(double time) const {
  return control_times_.contains(time);
}

// =========================================================
Model::Well Model::getWell(QString well_name) {

  // -------------------------------------------------------
  for (int wnr = 0; wnr < wells_.size(); ++wnr) {
    if(wells_.at(wnr).name.compare(well_name)) {
      return wells_.at(wnr);
    }
  }
}

}

