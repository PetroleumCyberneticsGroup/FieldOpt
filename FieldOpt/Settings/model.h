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
#include "trajectory_importer.h"

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
  enum WellType : int { Injector=11, Producer=12, UNKNOWN_TYPE=19 };
  enum ControlMode : int { BHPControl=21, RateControl=22, UNKNOWN_CONTROL=29 };
  enum InjectionType : int { WaterInjection=31, GasInjection=32 };
  enum WellDefinitionType : int { WellBlocks=41, WellSpline=42, PseudoContVertical2D=43, PolarSpline=45, UNDEFINED=44 };
  enum WellCompletionType : int { Perforation=61, ICV=62, Packer=63, Tubing=64, Annulus=65 };
  enum WellState : int { WellOpen=71, WellShut=72 };
  enum PreferredPhase : int { Oil=81, Water=82, Gas=83, Liquid=84, UNKNOWN_PHASE=89 };
  enum Direction : int { X=91, Y=92, Z=93 };

  struct Well {
    Well(){}
    struct Completion {
      Completion(){}
      WellCompletionType type;               //!< Which type of completion this is (Perforation/ICD)
      double transmissibility_factor =  0.0; //!< The transmissibility factor for this completion (used for perforations)
      double valve_size              = -1.0; //!< Valve size for nozzle ICDs.
      double valve_flow_coeff        = -1.0; //!< Dimensionless flow coefficient for valve.
      double diameter                = -1.0; //!< Diameter (for any completion in the segmented model)
      double cross_sect_area         = -1.0; //!< Cross sectional flow area
      double roughness               = -1.0; //!< Roughness (for any completion in the segmented model)
      double placement               = -1.0; //!< Placement as a fraction of the well length.
      double true_vertical_depth     = -1.0; //!< True vertical depth for the location of the completion.
      int time_step                  = -1;   //!< Time step for completion to activate/be modified.
      int segment_index              = -1;    //!< Segment index for the ICD in the deck (used when we don't specify the trajectory)
      std::string device_name        = "UNSET"; //!< Device name. Must be defined when doing ICV optimization with IX.
      std::vector<std::string> device_names; //< List of device names that have common settings.
      std::vector<int> segment_indexes; //!< List of segment indexes matching the device_names list.
      double min_valve_size         = 0.0;    //!< Minimum valve size (needed for simulator input)
      double max_valve_size         = 7.854E-3; //!< Maximum valve size (needed for simulator input)
      bool is_variable               = false; //!< True if all viable properties are variable. Otherwise false.
      bool variable_placement        = false; //!< Whether the placement of a comp. along the trajectory should be variable.
      bool variable_strength         = false; //!< Whether the strength of a comp. (e.g. ICD/perforation) should be variable.
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
    struct PolarSpline{
        SplinePoint midpoint;
        double azimuth=0;
        double length=0;
        double elevation=90;
        QString name="";
        bool is_variable=false;
    };
    struct PseudoContPosition {
      int i, j;
      bool is_variable = false;
    };
    struct ControlEntry {
      int time_step;                                 //!< The time step this control is to be applied at.
      WellState state;                               //!< Whether the well is open or shut.
      ControlMode control_mode;                      //!< Control mode.
      double bhp=-1;                                    //!< Bhp target when well is on bhp control.
      double rate=-1;                                   //!< Rate target when well is on rate control.
      InjectionType injection_type = WaterInjection; //!< Injector type (water/gas). Defaults to water.
      bool is_variable = false;
      QString name;
      bool isDifferent(ControlEntry other);
      std::string toString();
    };
    PreferredPhase preferred_phase;             //!< The preferred phase for the well
    QString name;                               //!< The name to be used for the well.
    WellType type;                              //!< The well type, i.e. producer or injector.
    QString group;                              //!< The group of the well.
    double wellbore_radius;                     //!< The wellbore radius
    Direction direction;                        //!< Direction of penetration
    WellDefinitionType definition_type=UNDEFINED; //!< How the well path is defined.
    QList<WellBlock> well_blocks;               //!< Well blocks when the well path is defined by WellBlocks.
    SplinePoint spline_heel;                    //!< Heel (start) point to be used when calculating the well path from a spline.
    SplinePoint spline_toe;                     //!< Toe (end) point to be used when calculating the well path from a spline.
    PolarSpline polar_spline;                   //!< PolarSpline contains azimuth, elevation, length and a spline point that together defines a well path
    QList<SplinePoint> spline_points;           //!< List of spline points, including heel (first) and toe (last).
    int n_spline_points = 0;                    //!< Number of points the spline is defined by. Used for conversion from list of well blocks.
    bool use_bezier_spline = false;            //!< Create a Bezier spline from the spline points to represent the well (only relevant for multiple points)
    bool convert_well_blocks_to_spline = false; //!< Whether or not the list of well blocks should be converted to a spline. This will be done in the Model initialization step.
    bool is_variable_spline;                    //!< Whether the whole spline should be variable.
    PseudoContPosition pseudo_cont_position;    //!< Initial position when using pseudo-continous positioning variables.
    QList<ControlEntry> controls;               //!< List of well controls
    bool use_segmented_model = false;           //!< Whether the segmented well model should be used.
    Completion seg_tubing;                      //!< Tubing settings when the segmented well model is used.
    Completion seg_annulus;                     //!< Annulus settings when the segmented well model is used.
    Completion seg_compartment_params;          //!< Parameters to be used for automatically generated ICDs.
    std::vector<Completion> completions;        //!< List of completions. Used when using neither segmentation or trajectories.
    int seg_n_compartments = 0;                 //!< Number of packer-delimited compartments with ICDs to use.
    std::vector<TrajectoryImporter::ImportedWellBlock> imported_wellblocks_; //!< List of imported well blocks.
    std::string toString();
  };

  QList<Well> wells() const { return wells_; }                //!< Get the struct containing settings for the well(s) in the model.
  QList<int> control_times() const { return control_times_; } //!< Get the control times for the schedule

 private:
  QList<Well> wells_;
  QList<int> control_times_;

  void readReservoir(QJsonObject json_reservoir, Paths &paths);
  Well readSingleWell(QJsonObject json_well);
  void setImportedWellDefaults(QJsonObject json_model);
  void parseImportedWellOverrides(QJsonArray json_wells);

  void parseSegmentation(QJsonObject json_seg, Well &well);
  void parseSegmentTubing(const QJsonObject &json_seg, Well &well) const;
  void parseSegmentAnnulus(const QJsonObject &json_seg, Well &well) const;
  void parseSegmentCompartments(const QJsonObject &json_seg, Well &well) const;

  void parseICVs(QJsonArray &json_icvs, Well &well);

  bool controlTimeIsDeclared(int time) const;

};

}


#endif // SETTINGS_MODEL_H
