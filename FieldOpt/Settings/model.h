/******************************************************************************
 *
 * model.h
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

#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

#include "settings.h"
#include "paths.h"

#include <QString>
#include <QList>
#include <QJsonArray>
#include <assert.h>


namespace Settings {

class DeckParser;

/*!
 * \brief The Model class contains model-specific settings. Model settings objects may _only_ be
 * created by the Settings class. They are created when reading a JSON-formatted "driver file".
 *
 * \todo Add more control types, e.g. multiple rate types (liquid rate, oil rate, gas rate, etc.) for producers.
 */
class Model
{
  friend class Settings;

 public:
  Model(QJsonObject json_model, Paths &paths); // This should only be accessed externally for testing purposes.
  enum ReservoirGridSourceType : int { ECLIPSE=1 };
  enum WellType : int { Injector=11, Producer=12, UNKNOWN_TYPE=19 };
  enum ControlMode : int { BHPControl=21, RateControl=22, UNKNOWN_CONTROL=29 };
  enum InjectionType : int { WaterInjection=31, GasInjection=32 };
  enum WellDefinitionType : int { WellBlocks=41, WellSpline=42, PseudoContVertical2D=43 };
  enum WellCompletionType : int { Perforation=61 };
  enum WellState : int { WellOpen=71, WellShut=72 };
  enum PreferredPhase : int { Oil=81, Water=82, Gas=83, Liquid=84, UNKNOWN_PHASE=89 };
  enum Direction : int { X=91, Y=92, Z=93 };

  struct Reservoir {
    ReservoirGridSourceType type; //!< The source of the grid file (which reservoir simulator produced it).
  };

  struct Well {
    Well(){}
    struct Completion {
      Completion(){}
      WellCompletionType type; //!< Which type of completion this is (Perforation/ICD)
      double transmissibility_factor; //!< The transmissibility factor for this completion (used for perforations)
      bool is_variable = false;
      QString name;
    };
    struct WellBlock {
      WellBlock(){}
      bool is_variable;
      bool has_completion;
      Completion completion;
      QString name;
      int i, j, k;
    };
    struct SplinePoint {
      SplinePoint(){}
      QString name;
      double x, y, z;
      bool is_variable = false;
    };
    struct PseudoContPosition {
      int i, j;
      bool is_variable = false;
    };
    struct ControlEntry {
      int time_step; //!< The time step this control is to be applied at.
      WellState state; //!< Whether the well is open or shut.
      ControlMode control_mode; //!< Control mode.
      double bhp; //!< Bhp target when well is on bhp control.
      double rate; //!< Rate target when well is on rate control.
      InjectionType injection_type = WaterInjection; //!< Injector type (water/gas). Defaults to water.
      bool is_variable = false;
      QString name;
      bool isDifferent(ControlEntry other);
      std::string toString();
    };
    PreferredPhase preferred_phase; //!< The preferred phase for the well
    QString name; //!< The name to be used for the well.
    WellType type; //!< The well type, i.e. producer or injector.
    QString group; //!< The group of the well.
    double wellbore_radius; //!< The wellbore radius
    Direction direction; //!< Direction of penetration
    WellDefinitionType definition_type; //!< How the well path is defined.
    QList<WellBlock> well_blocks; //!< Well blocks when the well path is defined by WellBlocks.
    SplinePoint spline_heel; //!< Heel (start) point to be used when calculating the well path from a spline.
    SplinePoint spline_toe; //!< Toe (end) point to be used when calculating the well path from a spline.
    QList<SplinePoint> spline_points; //!< List of spline points, including heel (first) and toe (last).
    bool is_variable_spline; //!< Whether the whole spline should be variable.
    PseudoContPosition pseudo_cont_position; //!< Initial position when using pseudo-continous positioning variables.
    QList<ControlEntry> controls; //!< List of well controls
    std::string toString();
  };

  Reservoir reservoir() const { return reservoir_; } //!< Get the struct containing reservoir settings.
  QList<Well> wells() const { return wells_; } //!< Get the struct containing settings for the well(s) in the model.
  QList<int> control_times() const { return control_times_; } //!< Get the control times for the schedule

 private:
  Reservoir reservoir_;
  QList<Well> wells_;
  QList<int> control_times_;
  DeckParser *deck_parser_;

  void readReservoir(QJsonObject json_reservoir, Paths &paths);
  Well readSingleWell(QJsonObject json_well);
  void setImportedWellDefaults(QJsonObject json_model);
  void parseImportedWellOverrides(QJsonArray json_wells);

  bool controlTimeIsDeclared(int time) const;

  /*!
   * Get the control time that is closest to the time imported from the deck.
   */
  int getClosestControlTime(int deck_time);
};

}


#endif // SETTINGS_MODEL_H
