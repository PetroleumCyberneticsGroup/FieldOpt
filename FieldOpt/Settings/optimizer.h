/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef SETTINGS_OPTIMIZER_H
#define SETTINGS_OPTIMIZER_H

#include "settings.h"

#include <QList>
#include <QString>
#include <QStringList>

namespace Settings {

/*!
 * \brief The Optimizer class contains optimizer-specific
 * settings. Optimizer settings objects may _only_ be
 * created by the Settings class. They are created when
 * reading a JSON-formatted "driver file".
 */
class Optimizer
{
  friend class Settings;

 public:
  Optimizer(){}
  Optimizer(QJsonObject json_optimizer);

  enum OptimizerType {
    Compass, APPS, ExhaustiveSearch2DVert,
    GeneticAlgorithm, WISmoothnessTest };

  enum OptimizerMode { Maximize, Minimize };

  enum ConstraintType {
    BHP, Rate, SplinePoints, WellSplineLength,
    WellSplineInterwellDistance, WellSplineDomain,
    CombinedWellSplineLengthInterwellDistance,
    CombinedWellSplineLengthInterwellDistanceReservoirBoundary,
    ReservoirBoundary
  };

  enum ConstraintWellSplinePointsType { MaxMin, Function};

  enum ObjectiveType { WeightedSum };

  struct Parameters {

    // GSS parameters
    // --------------
    /*! Maximum number of evaluations allowed
        before terminating the optimization run. */
    int max_evaluations;

    /*! The initial step length in the algorithm
        when applicable. */
    double initial_step_length;

    /*! The minimum step length in the algorithm
        when applicable. */
    double minimum_step_length;

    /// The contraction factor for GSS algorithms.
    double contraction_factor;

    /// The expansion factor for GSS algorithms.
    double expansion_factor;

    /// Maximum size of evaluation queue.
    double max_queue_size;

    /// The pattern to be used for GSS algorithms.
    QString pattern;


    // GA parameters
    // -------------
    /// Max iterations. Default: 50
    int max_generations;

    /*! Optional. Can be determined automatically.
        Default: min(10*nvars, 100). */
    int population_size;

    /*! Fraction to be discarded during selection.
        Defaults: 1/population. */
    double discard_parameter;

    /// Crossover probability. Default: 0.1.
    double p_crossover;

    /// Decay rate. Default: 4.0.
    double decay_rate;

    /// Mutation strength. Default: 0.25.
    double mutation_strength;

    /// Stagnation limit. Default: 1e-10.
    double stagnation_limit;

    /*! Simple lower bound. This is applied 
        to _all_ variables. Default: -10.0. */
    double lower_bound;

    /*! Simple upper bound. This is applied 
        to _all_ variables. Default: +10.0. */
    double upper_bound;
  };

  struct Objective {
    /// Objective definition type (e.g. WeightedSum)
    ObjectiveType type; 

    struct WeightedSumComponent { 
      double coefficient; QString property; 
      int time_step;
      /*! A component of a weighted sum 
          formulatied objective function */
      bool is_well_prop; QString well; 
    }; 

    /*! The expression for the Objective 
        function formulated as a weighted sum */
    QList<WeightedSumComponent> weighted_sum; 
  };

  struct Constraint {
    /// Used to express (x,y,z) coordinates.
    struct RealCoordinate
    { double x; double y; double z; }; 

    /*! Used to define a box-shaped 3D area. 
        Max and min each define a corner. */
    struct RealMaxMinLimit 
    { RealCoordinate max; RealCoordinate min; }; 

    /// The constraint type (e.g. BHP or SplinePoints positions).
    ConstraintType type; 
    
    /// The name of the well this Constraint applies to.
    QString well; 

    /// Well name list if constraint applies to more than one.
    QStringList wells; 

    /// Max limit when using constraints like BHP.
    double max;

    /// Min limit when using constraints like BHP.
    double min; 

    /// Min max limits for geometrix box constraints.
    double 
    box_imin, box_imax, 
    box_jmin, box_jmax, 
    box_kmin, box_kmax; 

    double max_length;
    double min_length;
    double min_distance;
    int max_iterations;

    /*! How the SplinePoints constraint is given 
        when SplinePoints constraint type is selected. */
    ConstraintWellSplinePointsType spline_points_type; 

    /*! Box limits a spline point needs to be within 
        to be valid when SplinePoints constraint type 
        is selected. */
    QList<RealMaxMinLimit> spline_points_limits; 
  };

  /// Get the Optimizer type (e.g. Compass).
  OptimizerType type() const { return type_; } 
 
  /// Get the optimizer mode (maximize/minimize).
  OptimizerMode mode() const { return mode_; } 

  /// Get the optimizer parameters.
  Parameters parameters() const { return parameters_; } 

  /// Get the optimizer objective function.
  Objective objective() const { return objective_; } 

  /// Get the optimizer constraints.
  QList<Constraint> constraints() const { return constraints_; } 

 private:
  QList<Constraint> constraints_;
  OptimizerType type_;
  Parameters parameters_;
  Objective objective_;
  OptimizerMode mode_;
  Constraint parseSingleConstraint(QJsonObject json_constraint);
};

}

#endif // SETTINGS_OPTIMIZER_H
