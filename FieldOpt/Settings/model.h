/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 Created: 28.09.2015 2015 by einar

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

// ---------------------------------------------------------
#include "settings.h"

// ---------------------------------------------------------
// QT / STD
#include <QString>
#include <QList>
#include <QJsonArray>
#include <assert.h>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>

// ---------------------------------------------------------
using std::string;
using std::pair;
using std::vector;
using std::multimap;
using std::cout;
using std::endl;

// ---------------------------------------------------------
namespace Settings {

class DeckParser;

// =========================================================
/*!
 * \brief The Model class contains model-specific
 * settings. Model settings objects may _only_ be
 * created by the Settings class. They are created
 * when reading a JSON-formatted "driver file".
 *
 * \todo Add more control types, e.g. multiple rate
 * types (liquid rate, oil rate, gas rate, etc.) for
 * producers.
 *
 */
class Model
{
  // -------------------------------------------------------
  friend class Settings;

 public:
  // -------------------------------------------------------
  // This should only be accessed externally
  // for testing purposes.
  // Model(QJsonObject json_model,
  //      vector<int> verb_vector);

  Model(QJsonObject json_model,
        QString schedule_path,
        vector<int> verb_vector);

  // -------------------------------------------------------
  enum ReservoirGridSourceType : int { ECLIPSE=1 };

  inline const static QString ResTypeStr(ReservoirGridSourceType resType) {
    switch (resType) {
      case ECLIPSE : return "ECLIPSE";
    }
  }

  // -------------------------------------------------------
  enum WellType : int { Injector=011, Producer=12, UNKNOWN_TYPE=19  };

  inline const static QString WellTypeStr(WellType wellType) {
    switch (wellType) {
      case Injector : return "Inj";
      case Producer : return "Prod";
    }
  }

  // -------------------------------------------------------
  enum ControlMode : int { BHPControl=21, RateControl=22, UNKNOWN_CONTROL=29  };

  inline const static QString ControlModeStr(ControlMode controlMode) {
    switch (controlMode) {
      case BHPControl : return "BHP";
      case RateControl : return "Rate";
    }
  }

  // -------------------------------------------------------
  enum InjectionType : int { WaterInjection=31, GasInjection=32 };

  inline const static QString InjTypeStr(InjectionType injType) {
    switch (injType) {
      case WaterInjection : return "WatInj";
      case GasInjection : return "GasInj";
    }
  }

  // -------------------------------------------------------
  enum WellDefinitionType : int { WellBlocks=41,
    WellSpline=42, PseudoContVertical2D=43 };

  inline const static QString WellDefTypeStr(WellDefinitionType wellDefType) {
    switch (wellDefType) {
      case WellBlocks : return "WellBlocks";
      case WellSpline : return "WellSpline";
      case PseudoContVertical2D : return "PseudoContVertical2D";
    }
  }

  // -------------------------------------------------------
  enum WellCompletionType : int { Perforation=61 };

  inline const static QString WellCompTypeStr(WellCompletionType wellCompType) {
    switch (wellCompType) {
      case Perforation : return "Perforation";
    }
  }

  // -------------------------------------------------------
  enum WellState : int {
    WellOpen=71, WellShut=72 };

  inline const static QString WellStateStr(WellState wellState) {
    switch (wellState) {
      case WellOpen : return "Open";
      case WellShut : return "Shut";
    }
  }

  // -------------------------------------------------------
  enum PreferredPhase : int {
    Oil=81, Water=82, Gas=83, Liquid=84, UNKNOWN_PHASE=89 };

  inline const static QString PreferredPhaseStr(PreferredPhase prefPhase) {
    switch (prefPhase) {
      case Oil : return "Oil";
      case Water : return "Water";
      case Gas : return "Gas";
      case Liquid : return "Liquid";
    }
  }

  // -------------------------------------------------------
  enum Direction : int { X=91, Y=92, Z=93 };

  // -------------------------------------------------------
  enum DrillingMode : int {
    Synchronous=101, Sequential=102 };

  inline const static QString DrillingStr(DrillingMode drilling) {
    switch (drilling) {
      case Synchronous : return "Synchronous";
      case Sequential : return "Sequential";
    }
  }

  // -------------------------------------------------------
  struct Reservoir {
    // Source of grid file (i.e., which simulator produced it).
    ReservoirGridSourceType type;

    // Path to .EGRID or .GRID file produced by ECLIPSE.
    QString path;
  };

  // -------------------------------------------------------
  struct Well {
    Well(){}

    // -----------------------------------------------------
    struct Completion {
      Completion(){}
      // Type of completion (Perforation/ICD)
      WellCompletionType type;
      // Transmissibility factor for completion (used for perforations)
      double transmissibility_factor;
      bool is_variable;
      QString name;
    };

    // -----------------------------------------------------
    struct WellBlock {
      WellBlock(){}
      bool is_variable;
      bool has_completion;
      Completion completion;
      QString name;
      int i, j, k;
    };

    // -----------------------------------------------------
    struct SplinePoint {
      SplinePoint(){}
      QString name;
      double x, y, z;
      bool is_variable = false;
    };

    // -----------------------------------------------------
    struct PseudoContPosition {
      int i, j;
      bool is_variable = false;
    };

    // -----------------------------------------------------
    struct ControlEntry {

      // The time step this control is to be applied at.
      double time_step = -1.0;

      WellState state = WellOpen; // The time step this control
      // is to be applied at. Whether the well is open or shut.

      // Control mode.
      ControlMode control_mode = BHPControl;

      // Bhp target when well is on bhp control.
      double bhp = -1.0;

      // Rate target when well is on rate control.
      double rate = -1.0;

      // Injector type (water/gas)
      InjectionType injection_type = WaterInjection;

      bool is_variable = false;

      QString name = "NONE";

      bool isDifferent(ControlEntry other);

      std::string toString();

    };

    // -----------------------------------------------------
    // The preferred phase for the well
    PreferredPhase preferred_phase;

    // The name to be used for the well.
    QString name;

    // The well type, i.e. producer or injector.
    WellType type;

    // -----------------------------------------------------
    // The wellbore radius
    double wellbore_radius;

    // The group of the well.
    QString group;

    // -----------------------------------------------------
    // Drilling sequence for each well
    mutable double drilling_time;
    std::pair<int, int> drilling_order;

    double GetDrillingTime() const
    { return drilling_time; };

    std::pair<int, int> GetDrillingOrder() const
    { return drilling_order; };

    // -----------------------------------------------------
    // Direction of penetration
    Direction direction;

    // How the well path is defined.
    WellDefinitionType definition_type;

    // Well blocks when well path is defined by WellBlocks.
    QList<WellBlock> well_blocks;

    // -------------------------------------------------------
    // Heel (start) point used when
    // calculating well path from spline.
    SplinePoint spline_heel;

    // Toe (end) point used when calculating
    // well path from spline.
    SplinePoint spline_toe;

    // -------------------------------------------------------
    // Initial position when using
    // pseudo-continous positioning variables.
    PseudoContPosition pseudo_cont_position;

    // List of well controls
    QList<ControlEntry> controls;

    std::string toString();

    // -------------------------------------------------------
    vector<int> verb_vector() const { return verb_vector_; } // DELETE
    QString output_directory_;
    vector<int> verb_vector_ = vector<int>(11,0);

  };

  // -------------------------------------------------------
  inline static QString ControlStr(Well::ControlEntry c_entry) {

    QString str;
    str += " [N: " + c_entry.name;
    str += "] [TS: " + QString::number(c_entry.time_step);
    str += "] [ST: " + WellStateStr(c_entry.state);
    str += "] [M: " + ControlModeStr(c_entry.control_mode);
    str += "] [BHP: " + QString::number(c_entry.bhp);
    str += "] [RATE: " + QString::number(c_entry.rate);
    str += "] [IN: " + InjTypeStr(c_entry.injection_type);
    str += "] [V: " + QString::number(c_entry.is_variable) + "]";
    return str;
  }

  // -------------------------------------------------------
  // Get the struct containing reservoir settings.
  Reservoir reservoir() const { return reservoir_; }

  // Set the reservoir grid path. Used when
  // path is passed by command line argument.
  void set_reservoir_grid_path(const QString path)
  { reservoir_.path = path; }

  // -------------------------------------------------------
  // Get the struct containing settings for the well(s) in
  // the model.
  QList<Well> wells() const { return wells_; }

  // Get the control times for the schedule
  QList<double> control_times() const
  { return control_times_; }

  // -------------------------------------------------------
  void sort_control_steps() {
    std::sort(control_times_.begin(),
              control_times_.end());
  }

  // -------------------------------------------------------
  Model::Well getWell(QString well_name);

  DrillingMode drillingMode_;

  // -------------------------------------------------------
 public:
  QString output_directory_;
  vector<int> verb_vector_ = vector<int>(11,0);

 private:
  // -------------------------------------------------------
  Reservoir reservoir_;
  QList<Well> wells_;
  QList<double> control_times_;
  DeckParser *deck_parser_;

  // -------------------------------------------------------
  void readReservoir(QJsonObject json_reservoir);
  Well readSingleWell(QJsonObject json_well);
  void setImportedWellDefaults(QJsonObject json_model);
  void parseImportedWellOverrides(QJsonArray json_wells);


  // -------------------------------------------------------
  bool controlTimeIsDeclared(double time) const;

  /*!
   * Get the control time that is closest
   * to the time imported from the deck.
   */
  int getClosestControlTime(int deck_time);
};

}

#endif // SETTINGS_MODEL_H
