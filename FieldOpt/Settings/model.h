/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>
   Created: 28.09.2015 2015 by einar

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt. If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

#include "settings.h"

#include <QString>
#include <QList>
#include <QJsonArray>
#include <assert.h>

namespace Settings {

/*!
 * \brief The Model class contains model-specific
 * settings. Model settings objects may _only_ be
 * created by the Settings class. They are created
 * when reading a JSON-formatted "driver file".
 *
 * \todo Add more control types, e.g. multiple rate
 * types (liquid rate, oil rate, gas rate, etc.) for
 * producers.
 */
class Model
{
  friend class Settings;

 public:
  enum ReservoirGridSourceType : int { ECLIPSE=1 };
  enum WellType : int { Injector=011, Producer=12 };
  enum ControlMode : int { BHPControl=21, RateControl=22 };
  enum InjectionType : int { WaterInjection=31, GasInjection=32 };
  enum WellDefinitionType : int { WellBlocks=41, WellSpline=42 };
  enum WellCompletionType : int { Perforation=61 };
  enum WellState : int { WellOpen=71, WellShut=72 };
  enum PreferredPhase : int { Oil=81, Water=82, Gas=83, Liquid=84 };
  enum Direction : int { X=91, Y=92, Z=93 };
  enum WellModel : int { Peaceman=101, Projection=102 };

  struct Reservoir {
    /// Grid file source (which reservoir simulator produced it).
    ReservoirGridSourceType type;

    /*! Path to the reservoir grid file, e.g. a
     * .EGRID or .GRID file produced by ECLIPSE. */
    QString path;
  };

  struct Well {
    Well(){}

    struct Completion {
      Completion(){}
      /// Which type of completion this is (Perforation/ICD)
      WellCompletionType type;
      /// Transmissibility factor for completion (used for perforations)
      double transmissibility_factor;

      bool is_variable;
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
      bool is_variable;
    };

    struct ControlEntry {
      /// The time step this control is to be applied at.
      int time_step;
      /// Whether the well is open or shut.
      WellState state;

      /// Control mode.
      ControlMode control_mode;
      /// Bhp target when well is on bhp control.
      double bhp;
      /// Rate target when well is on rate control.
      double rate;
      /// Injector type (water/gas)
      InjectionType injection_type;
      bool is_variable;
      QString name;
    };

    /// The preferred phase for the well
    PreferredPhase preferred_phase;
    /// The name to be used for the well.
    QString name;
    /// The well type, i.e. producer or injector.
    WellType type;
    /// The group of the well.
    QString group;

    /// Type of well model for wi calculation
    WellModel well_model;
    /// The wellbore radius
    double wellbore_radius;
    /// Direction of penetration
    Direction direction;
    /// How the well path is defined.
    WellDefinitionType definition_type;
    /*! Well blocks when the well path
        is defined by WellBlocks. */
    QList<WellBlock> well_blocks;

    /*! Heel (start) point to be used when
        calculating the well path from a spline. */
    SplinePoint spline_heel;
    /*! Toe (end) point to be used when
        calculating the well path from a spline. */
    SplinePoint spline_toe;
    /// List of well controls
    QList<ControlEntry> controls;

  };

  /// Get the struct containing reservoir settings.
  Reservoir reservoir() const { return reservoir_; }
  /*! Set the reservoir grid path. Used when
  the path is passed by command line argument. */
  void set_reservoir_grid_path(const QString path) { reservoir_.path = path; }
  /*! Get the struct containing settings
  for the well(s) in the model. */
  QList<Well> wells() const { return wells_; }
  /// Get the control times for the schedule
  QList<int> control_times() const { return control_times_; }

 private:
  Model(QJsonObject json_model);
  Reservoir reservoir_;
  QList<Well> wells_;
  QList<int> control_times_;


  void readReservoir(QJsonObject json_reservoir);
  Well readSingleWell(QJsonObject json_well);

  bool controlTimeIsDeclared(int time) const;
};

}


#endif // SETTINGS_MODEL_H
