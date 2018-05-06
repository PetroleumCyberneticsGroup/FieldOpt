/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#ifndef SETTINGS_OPTIMIZER_H
#define SETTINGS_OPTIMIZER_H

// ---------------------------------------------------------
#include "settings.h"
#include "simulator.h"

// ---------------------------------------------------------
#include <QList>
#include <QString>
#include <QStringList>
#include <FieldOpt-WellIndexCalculator/resinxx/rixx_core_geom/cvfBoundingBox.h>

// ---------------------------------------------------------
namespace Settings {

// =========================================================
/*!
 * \brief The Optimizer class contains optimizer-specific
 * settings. Optimizer settings objects may _only_ be
 * created by the Settings class. They are created when
 * reading a JSON-formatted "driver file".
 *
 */
class Optimizer
{
  friend class Settings;

 public:
  // -------------------------------------------------------
  Optimizer(){}
  Optimizer(QJsonObject json_optimizer);

  // -------------------------------------------------------
  enum OptimizerType {
    Compass, APPS, ExhaustiveSearch2DVert,
    GeneticAlgorithm, EGO, SNOPTSolver, DFO };

  // -------------------------------------------------------
  enum OptimizerMode { Maximize, Minimize };

  // -------------------------------------------------------
  enum ConstraintType {
    IWD, ADG, BHP, Rate, SplinePoints, WellSplineLength,
    WellSplineInterwellDistance, WellSplineDomain,
    CombinedWellSplineLengthInterwellDistance,
    CombinedWellSplineLengthInterwellDistanceReservoirBoundary,
    ReservoirBoundary, PseudoContBoundary2D
  };

  // -------------------------------------------------------
  enum ConstraintWellSplinePointsType { MaxMin, Function};

  // -------------------------------------------------------
  enum ObjectiveType { WeightedSum };

  // -------------------------------------------------------
//  struct Runtime {
//    QString output_dir_;
//  };

  // -------------------------------------------------------
  struct SimDirs {

    QString driver_file_path_;
    QString driver_directory_;
    QString output_directory_;

    QString sim_incl_dir_path_;
    QString schedule_file_path_;
    QString custom_exec_script_path_;

  };

  SimDirs sim_dirs_;

  SimDirs GetSimDirs() { return sim_dirs_; }

//  void UpdateSimDirs(Simulator *settings){
//
//    sim_dirs_.driver_file_path_ = settings->driver_file_path();
//    sim_dirs_.driver_directory_ = settings->driver_parent_directory();
//
//  };

  // -------------------------------------------------------
  struct Parameters {

    // [GSS parameters]

    // Max # of evals allowed b/e terminating optimization.
    int max_evaluations;

    // contraction factor for GSS algorithms.
    double contraction_factor;

    // Expansion factor for GSS algorithms.
    double expansion_factor;

    // Maximum size of evaluation queue.
    double max_queue_size;

    // Pattern to be used for GSS algorithms.
    QString pattern;

    // Initial step length when applicable.
    double initial_step_length;

    // XYZ scaled initial step length
    QList<double > initial_step_length_xyz;

    // Min step length when applicable.
    double minimum_step_length;

    // XYZ scaled minimum step length
    QList<double > minimum_step_length_xyz;

    // -------------------------------------------------------
    // [GA parameters]

    // Max iterations. Default: 50
    int max_generations;

    // Optional. Can be determined automatically.
    // Def: min(10*nvars, 100).
    int population_size;

    // Fraction to be discarded during selection.
    // Def: 1/population.
    double discard_parameter;

    // Crossover probability. Def: 0.1.
    double p_crossover;

    // Decay rate. Default: 4.0.
    double decay_rate;

    // Mutation strength. Def: 0.25.
    double mutation_strength;

    // Stagnation limit. Def: 1e-10.
    double stagnation_limit;

    // Simple lower bound. Applied to _all_ variables.
    // Def: -10.0.
    double lower_bound;

    // Simple upper bound. Applied to _all_ variables.
    // Def: +10.0.
    double upper_bound;

    // -------------------------------------------------------
    // DFO parameters
    double initial_trust_region_radius;

    // -------------------------------------------------------
    // SNOPT solver parameters
    QString thrdps_optn_file;
    QString thrdps_smry_file;
    QString thrdps_prnt_file;

  };

  // -------------------------------------------------------
  struct Objective {
    // The objective definition type (e.g., WeightedSum)
    ObjectiveType type;

    // Whether or not to use penalty function (def: false).
    bool use_penalty_function;

    struct WeightedSumComponent {

      double coefficient; QString property;
      int time_step;

      // A component of a weighted sum
      // formulatied objective function
      bool is_well_prop; QString well; };

    // The expression for the Objective
    // function formulated as a weighted sum
    QList<WeightedSumComponent> weighted_sum;
  };


  // -------------------------------------------------------
  struct WplcRBBConstraint {

  };

  // -------------------------------------------------------
  struct WplcRRGConstraint {

  };

  // -------------------------------------------------------
  struct WplcIWDConstraint {

  };

  // -------------------------------------------------------
  struct Constraint {

    // Used to express (x,y,z) coordinates.
    struct RealCoordinate { double x; double y; double z; };

    // Used to define a box-shaped 3D area.
    // Max and min each define a corner.
    struct RealMaxMinLimit
    { RealCoordinate max; RealCoordinate min; };

    // Constraint type (e.g., BHP or SplinePoints positions).
    ConstraintType type;

    // Name of well this Constraint applies to.
    QString well;

    // List of well names if constraint
    // applies to more than one.
    QStringList wells;

    // Max limit when using constraints like BHP.
    double max;

    // Min limit when using constraints like BHP.
    double min;

    // Min max limits for geometric box constraints.
    double
        box_imin, box_jmin, box_kmin,
        box_imax, box_jmax, box_kmax;

    double max_length;
    double min_length;
    double min_distance;

    // Weight to be used when considering the
    // constraint in a penalty function. (def: 0.0)
    long double penalty_weight;

    int max_iterations;

    // How the SplinePoints constraint is given
    // when SplinePoints constraint type is selected.
    ConstraintWellSplinePointsType spline_points_type;

    // Box limits a spline point needs to be within
    // to be valid when SplinePoints constraint type
    // is selected.
    QList<RealMaxMinLimit> spline_points_limits;

    // -----------------------------------------------------
    // 3rd party solver parameters
    QString thrdps_optn_file;
    QString thrdps_smry_file;
    QString thrdps_prnt_file;

    std::vector<WplcRRGConstraint> rrg_constraints;
    std::vector<WplcRBBConstraint> rbb_constraints;
    std::vector<WplcIWDConstraint> iwd_constraints;

    // cvf::BoundingBox bbgrid_ = nullptr;

    // -----------------------------------------------------
    std::string exec_adg_const_name_;
    std::string exec_adg_const_path_;

    // -----------------------------------------------------
    std::vector<int> verb_vector_ = std::vector<int>(11,0);

  };

  // -------------------------------------------------------
  // Get the Optimizer type (e.g. Compass).
  OptimizerType type() const { return type_; }

  // Get the optimizer mode (maximize/minimize).
  OptimizerMode mode() const { return mode_; }

  // Get the optimizer parameters.
  Parameters parameters() const { return parameters_; }

  // Get the optimizer objective function.
  Objective objective() const { return objective_; }

  // Get the optimizer constraints.
  QList<Constraint> constraints() const { return constraints_; }

  // -------------------------------------------------------
  void set_verbosity_vector(const std::vector<int> verb_vector)
  { verb_vector_ = verb_vector; }

  std::vector<int> verb_vector() const { return verb_vector_; }

  // -------------------------------------------------------
  QString output_dir_;

 private:
  // -------------------------------------------------------
  QList<Constraint> constraints_;
  OptimizerType type_;
  Parameters parameters_;
  Objective objective_;
  OptimizerMode mode_;

  Constraint parseSingleConstraint(QJsonObject json_constraint);

  std::vector<int> verb_vector_ = std::vector<int>(11,0);
};

}


#endif // SETTINGS_OPTIMIZER_H
