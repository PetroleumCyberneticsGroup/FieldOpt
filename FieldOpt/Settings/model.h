/***********************************************************
 model.h

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

// ---------------------------------------------------------------
#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

// ---------------------------------------------------------------
#include "settings.h"

// ---------------------------------------------------------------
// QT
#include <QString>
#include <QList>
#include <QJsonArray>
#include <assert.h>

// ---------------------------------------------------------------
namespace Settings {

// ===============================================================
/*!
 * \brief The Model class contains model-specific settings.
 * Model settings objects may _only_ be created by the Settings
 * class. They are created when reading a JSON-formatted "driver
 * file".
 *
 * \todo Add more control types, e.g. multiple rate types
 * (liquid rate, oil rate, gas rate, etc.) for producers.
 */
class Model
{
  // -------------------------------------------------------------
  friend class Settings;

 public:
  // -------------------------------------------------------------
  // This should only be accessed externally for testing purposes.
  Model(QJsonObject json_model);

  // -------------------------------------------------------------
  enum ReservoirGridSourceType : int { ECLIPSE=1 };

  enum WellType : int { Injector=011, Producer=12 };

  enum ControlMode : int { BHPControl=21, RateControl=22 };

  enum InjectionType : int { WaterInjection=31, GasInjection=32 };

  enum WellDefinitionType : int { WellBlocks=41,
    WellSpline=42, PseudoContVertical2D=43 };

  enum WellCompletionType : int { Perforation=61 };

  enum WellState : int { WellOpen=71, WellShut=72 };

  enum PreferredPhase : int { Oil=81, Water=82, Gas=83, Liquid=84 };

  enum Direction : int { X=91, Y=92, Z=93 };

  // -------------------------------------------------------------
  struct Reservoir {
    // The source of the grid file (which reservoir simulator produced it).
    ReservoirGridSourceType type;

    // Path to reservoir grid file, e.g. a .EGRID or .GRID file produced by ECLIPSE.
    QString path;
  };

  // -------------------------------------------------------------
  struct Well {
    Well(){}

    // -----------------------------------------------------------
    struct Completion {
      Completion(){}
      WellCompletionType type; //!< Which type of completion this is (Perforation/ICD)
      double transmissibility_factor; //!< The transmissibility factor for this completion (used for perforations)
      bool is_variable;
      QString name;
    };

    // -----------------------------------------------------------
    struct WellBlock {
      WellBlock(){}
      bool is_variable;
      bool has_completion;
      Completion completion;
      QString name;
      int i, j, k;
    };

    // -----------------------------------------------------------
    struct SplinePoint {
      SplinePoint(){}
      QString name;
      double x, y, z;
      bool is_variable;
    };

    // -----------------------------------------------------------
    struct PseudoContPosition {
      int i, j;
      bool is_variable;
    };

    // -----------------------------------------------------------
    struct ControlEntry {
      double time_step; //!< The time step this control is to be applied at.
      WellState state; //!< Whether the well is open or shut.
      ControlMode control_mode; //!< Control mode.
      double bhp; //!< Bhp target when well is on bhp control.
      double rate; //!< Rate target when well is on rate control.
      InjectionType injection_type; //!< Injector type (water/gas)
      bool is_variable;
      QString name;
    };

    // -----------------------------------------------------------
    PreferredPhase preferred_phase; //!< The preferred phase for the well
    QString name; //!< The name to be used for the well.
    WellType type; //!< The well type, i.e. producer or injector.
    QString group; //!< The group of the well.

    // -----------------------------------------------------------
    double wellbore_radius; //!< The wellbore radius
    double drilling_time;
    std::vector<int> drilling_sequence;

    Direction direction; //!< Direction of penetration
    WellDefinitionType definition_type; //!< How the well path is defined.
    QList<WellBlock> well_blocks; //!< Well blocks when the well path is defined by WellBlocks.
    SplinePoint spline_heel; //!< Heel (start) point to be used when calculating the well path from a spline.
    SplinePoint spline_toe; //!< Toe (end) point to be used when calculating the well path from a spline.
    PseudoContPosition pseudo_cont_position; //!< Initial position when using pseudo-continous positioning variables.
    QList<ControlEntry> controls; //!< List of well controls
    std::vector<int> verb_vector_;
  };

  // -------------------------------------------------------------
  Reservoir reservoir() const { return reservoir_; } //!< Get the struct containing reservoir settings.
  void set_reservoir_grid_path(const QString path) { reservoir_.path = path; } //!< Set the reservoir grid path. Used when the path is passed by command line argument.

  // -------------------------------------------------------------
  QList<Well> wells() const { return wells_; } //!< Get the struct containing settings for the well(s) in the model.
  QList<double> control_times() const { return control_times_; } //!< Get the control times for the schedule

  // -------------------------------------------------------------
  void set_verbosity_vector(const std::vector<int> verb_vector) { verb_vector_ = verb_vector; }
  std::vector<int> verb_vector() const { return verb_vector_; }

 private:
  // -------------------------------------------------------------
  Reservoir reservoir_;
  QList<Well> wells_;
  QList<double> control_times_;

  // -------------------------------------------------------------
  void readReservoir(QJsonObject json_reservoir);
  Well readSingleWell(QJsonObject json_well);

  // -------------------------------------------------------------
  bool controlTimeIsDeclared(double time) const;

  // -------------------------------------------------------------
  std::vector<int> verb_vector_ = std::vector<int>(11,0); //!<
};

}


#endif // SETTINGS_MODEL_H
