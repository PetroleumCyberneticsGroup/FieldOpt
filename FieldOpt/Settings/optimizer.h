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
 * \brief The Optimizer class contains optimizer-specific settings. Optimizer settings objects
 * may _only_ be created by the Settings class. They are created when reading a
 * JSON-formatted "driver file".
 */
class Optimizer
{
  friend class Settings;

 public:
  Optimizer(){}
  Optimizer(QJsonObject json_optimizer);
  enum OptimizerType { Compass, APPS, ExhaustiveSearch2DVert, GeneticAlgorithm, EGO, PSO, VFSA, SPSA, Hybrid, CMA_ES };
  enum OptimizerMode { Maximize, Minimize };
  enum ConstraintType { BHP, Rate, SplinePoints,
    WellSplineLength, WellSplineInterwellDistance, WellSplineDomain,
    CombinedWellSplineLengthInterwellDistance,
    CombinedWellSplineLengthInterwellDistanceReservoirBoundary,
    ReservoirBoundary, PseudoContBoundary2D, PolarXYZBoundary,
    ReservoirXYZBoundary, ReservoirBoundaryToe,
    PackerConstraint, ICVConstraint, PolarWellLength,
    PolarAzimuth, PolarElevation, PolarSplineBoundary
  };
  enum ConstraintWellSplinePointsType { MaxMin, Function};
  enum ObjectiveType { WeightedSum, NPV, carbondioxidenpv };

  struct Parameters {
    // Common parameters
    int max_evaluations; //!< Maximum number of evaluations allowed before terminating the optimization run.
    int rng_seed;        //!< Seed to be used for random number renerators in relevant algorithms.

    // GSS parameters
    double initial_step_length; //!< The initial step length in the algorithm when applicable.
    double minimum_step_length; //!< The minimum step length in the algorithm when applicable.
    double contraction_factor;  //!< The contraction factor for GSS algorithms.
    double expansion_factor;    //!< The expansion factor for GSS algorithms.
    int max_queue_size;      //!< Maximum size of evaluation queue.
    bool auto_step_lengths = false;     //!< Automatically determine appropriate step lengths from bound constraints.
    double auto_step_init_scale = 0.25; //!< Scaling factor for auto-determined initial step lengths (e.g. 0.25*(upper-lower)
    double auto_step_conv_scale = 0.01; //!< Scaling factor for auto-determined convergence step lengths (e.g. 0.01*(upper-lower)
    QString pattern;                     //!< The pattern to be used for GSS algorithms.

    // GA parameters
    int max_generations;      //!< Max iterations. Default: 50
    int population_size;      //!< Optional. Can be determined automatically. Default: min(10*nvars, 100).
    double discard_parameter; //!< Fraction to be discarded during selection. Defaults: 1/population.
    double p_crossover;       //!< Crossover probability. Default: 0.1.
    double decay_rate;        //!< Decay rate. Default: 4.0.
    double mutation_strength; //!< Mutation strength. Default: 0.25.
    double stagnation_limit;  //!< Stagnation limit. Default: 1e-10.
    double lower_bound;       //!< Simple lower bound. This is applied to _all_ variables. Default: -10.0.
    double upper_bound;       //!< Simple upper bound. This is applied to _all_ variables. Default: +10.0.

    // PSO parameters
    double pso_learning_factor_1; //!< Learning factor (c1), from the swarms best known perturbation. Default: 2
    double pso_learning_factor_2; //!< Learning factor (c2), from the individual particle's best known perturbation. Default: 2
    double pso_swarm_size; //!< The number of particles in the swarm. Default: 50
    double pso_velocity_scale; //!< Scaling factor for particle velocities. Default: 1.0

    // EGO Parameters
    int ego_init_guesses = -1; //!< Number of initial guesses to be made (default is two times the number of variables).
    std::string ego_init_sampling_method = "Random"; //!< Sampling method to be used for initial guesses (Random or Uniform)
    std::string ego_kernel = "CovMatern5iso";        //!< Which kernel function to use for the gaussian process model.
    std::string ego_af = "ExpectedImprovement";      //!< Which acquisiton function to use.

    // VFSA Parameters
    int vfsa_evals_pr_iteration = 1; //!< Number of evaluations to be performed pr. iteration (temperature). Default: 1.
    int vfsa_max_iterations = 50;    //!< Maximum number of iterations to be performed. Default: 50.
    bool vfsa_parallel = false;      //!< Run generate evals_pr_iteration cases immedeately in each generation? Default: false.
    double vfsa_init_temp = 1.0;     //!< Initial temperature (same used for all dimensions). Default: 1.0.
    double vfsa_temp_scale = 1.0;    //!< Constant used in scaling temperature. Default: 1.0.

    // CMA-ES Parameters
    bool improve_base_case = false;

    // SPSA Parameters
    int spsa_max_iterations = 50; //!< Maximum number of iterations to be performed. Default: 50.
    double spsa_alpha = 0.602;    //!< Affects step length. Recommended 0.602 <= alpha <= 1.0. Default: 0.602.
    double spsa_gamma = 0.101;    //!< Affects step length. Recommended 0.101 <= gamma <= 1/6. Default: 0.101.
    double spsa_c = 1.0;          //!< Used to account for noise. Default: 1.0.
    double spsa_A = 5;            //!< Affects step length in early iterations. Default: 10% of max iterations.
    double spsa_a = 0.0;          //!< Affects step lengths. Default and recommended: automatically compute from spsa_init_step_magnitude.
    double spsa_init_step_magnitude = 0.0; //!< Smallest desired step magnitude in early iterations.

    // Hybrid parameters
    /*!
     * @brief How switching between component optimizers is handled.
     *
     * Default: OnFinished -- switch between components when IsFinished() == true
     *
     * Example: "Optimizer": { "Type": "Hybrid", "Parameters": { "HybridSwitchMode": "OnConvergence" } }
     */
    std::string hybrid_switch_mode = "OnConvergence";

    /*!
     * @brief Termination condition for hybrid optimizer.
     *
     * Default: NoImprovement -- terminate when a component has not managed to
     *                           improve upon the result of the previous component.
     *
     * Example: "Optimizer": { "Type": "Hybrid", "Parameters": { "HybridTerminationCondition": "NoImprovement" } }
     */
    std::string hybrid_termination_condition = "NoImprovement";

    /*!
     * @brief Max iterations for the hybrid optimizer.
     *
     * One iteration implies running each component to completion once.
     * If
     *
     * Default: 2 -- Each component will be executed twice (unleass the HybridTerminationCondition
     *               is met first).
     *
     * Example: "Optimizer": { "Type": "Hybrid", "Parameters": { "HybridMaxIterations": 2 } }
     */
    int hybrid_max_iterations = 2;
  };

  struct Objective {
    ObjectiveType type; //!< The objective definition type (e.g. WeightedSum, NPV)
    bool use_penalty_function; //!< Whether or not to use penalty function (default: false).
    bool use_well_cost; //!<Whether or not to use costs associated to wells in calculation of the objective.
    bool separatehorizontalandvertical; //!<Whether or not to use different values in the horizontal or vertical direction
    double wellCostXY; //!<Cost associated with drilling in the horizontal plane [$/m]
    double wellCostZ; //!<Cost associated with drilling in the vertical plane [$/m]
    double wellCost; //!<Cost associated with drilling the well, independent of direction [$/m]
    struct WeightedSumComponent {
      double coefficient; 
      QString property; 
      int time_step;
      bool is_well_prop; 
      QString well; 
    }; //!< A component of a weighted sum objective function
    struct NPVComponent{
      double coefficient;
      std::string property;
      std::string interval = "";
      bool usediscountfactor = false;
      std::string well;
      double discount = 0.0;
    };
    struct NPVCarbonComponent{
        //double coefficient;
        //double capacity;
        QString property;
        bool is_well_prop;
        QString well;
    };
    QList<WeightedSumComponent> weighted_sum; //!< The expression for the Objective function formulated as a weighted sum
    QList<NPVComponent> NPV_sum;  //!< The expression for the Objective function formulated as an NPV
    QList<NPVCarbonComponent> NPVCarbonComponents; //!< The expression for the CarbonComponent

  };

  struct Constraint {
    struct RealCoordinate { double x; double y; double z; }; //!< Used to express (x,y,z) coordinates.
    struct RealMaxMinLimit { RealCoordinate max; RealCoordinate min; }; //!< Used to define a box-shaped 3D area. Max and min each define a corner.
    ConstraintType type; //!< The constraint type (e.g. BHP or SplinePoints positions).
    QString well; //!< The name of the well this Constraint applies to.
    QStringList wells; //!< List of well names if the constraint applies to more than one.
    double max; //!< Max limit when using constraints like BHP.
    double min; //!< Min limit when using constraints like BHP.
    double box_imin, box_imax, box_jmin, box_jmax, box_kmin, box_kmax; //!< Min max limits for geometric box constraints.
    double box_xyz_xmin, box_xyz_ymin, box_xyz_zmin, box_xyz_xmax, box_xyz_ymax, box_xyz_zmax; //!< Min max limits for geometric xyz constraints.
    double min_length, max_length;
    double min_distance;
    double min_md, max_md;
    long double penalty_weight; //!< The weight to be used when considering the constraint in a penalty function. (default: 0.0)
    int max_iterations;
    ConstraintWellSplinePointsType spline_points_type; //!< How the SplinePoints constraint is given when SplinePoints constraint type is selected.
    QList<RealMaxMinLimit> spline_points_limits; //!< Box limits a spline point needs to be within to be valid when SplinePoints constraint type is selected.
  };

  struct HybridComponent {
    OptimizerType type;
    Parameters parameters;
  };
  Optimizer(HybridComponent hc); //!< Create a basic Optimizer Settings object from a HybridComponent object.

  OptimizerType type() const { return type_; } //!< Get the Optimizer type (e.g. Compass).
  OptimizerMode mode() const { return mode_; } //!< Get the optimizer mode (maximize/minimize).
  void set_mode(const OptimizerMode mode) { mode_ = mode; } //!< Set the optimizer mode (used by HybridOptimizer)
  Parameters parameters() const { return parameters_; } //!< Get the optimizer parameters.
  Objective objective() const { return objective_; } //!< Get the optimizer objective function.
  QList<Constraint> constraints() const { return constraints_; } //!< Get the optimizer constraints.
  QList<HybridComponent> HybridComponents() { return hybrid_components_; } // Get the list of hybrid-optimizer components when using the HYBRID type.
  void SetRngSeed(const int seed) { parameters_.rng_seed = seed; } //!< Change the RNG seed (used by HybridOptimizer).


 private:
  QList<Constraint> constraints_;
  OptimizerType type_;
  Parameters parameters_;
  Objective objective_;
  OptimizerMode mode_ = OptimizerMode::Maximize; //!< Optimization mode (maximize or minimize). Default: Maximize
  QList<HybridComponent> hybrid_components_;

  OptimizerType parseType(QString &type);
  Constraint parseSingleConstraint(QJsonObject json_constraint);
  OptimizerMode parseMode(QJsonObject &json_optimizer);
  Parameters parseParameters(QJsonObject &json_parameters);
  Objective parseObjective(QJsonObject &json_objective);
  QList<HybridComponent> parseHybridComponents(QJsonObject &json_optimizer);
};

}


#endif // SETTINGS_OPTIMIZER_H
