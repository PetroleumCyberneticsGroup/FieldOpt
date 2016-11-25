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
        enum OptimizerType { Compass, APPS, ExhaustiveSearch2DVert };
        enum OptimizerMode { Maximize, Minimize };
        enum ConstraintType { BHP, Rate, SplinePoints,
            WellSplineLength, WellSplineInterwellDistance, WellSplineDomain,
            CombinedWellSplineLengthInterwellDistance,
            CombinedWellSplineLengthInterwellDistanceReservoirBoundary,
            ReservoirBoundary
        };
        enum ConstraintWellSplinePointsType { MaxMin, Function};
        enum ObjectiveType { WeightedSum };

        struct Parameters {
            int max_evaluations; //!< Maximum number of evaluations allowed before terminating the optimization run.
            double initial_step_length; //!< The initial step length in the algorithm when applicable.
            double minimum_step_length; //!< The minimum step length in the algorithm when applicable.
            double contraction_factor; //!< The contraction factor for GSS algorithms.
            double expansion_factor; //!< The expansion factor for GSS algorithms.
            double max_queue_size; //!< Maximum size of evaluation queue.
            QString pattern; //!< The pattern to be used for GSS algorithms.
        };

        struct Objective {
            ObjectiveType type; //!< The objective definition type (e.g. WeightedSum)
            struct WeightedSumComponent {
                double coefficient; QString property; int time_step;
                bool is_well_prop; QString well; }; //!< A component of a weighted sum formulatied objective function
            QList<WeightedSumComponent> weighted_sum; //!< The expression for the Objective function formulated as a weighted sum
        };

        struct Constraint {
            struct RealCoordinate { double x; double y; double z; }; //!< Used to express (x,y,z) coordinates.
            struct RealMaxMinLimit { RealCoordinate max; RealCoordinate min; }; //!< Used to define a box-shaped 3D area. Max and min each define a corner.
            ConstraintType type; //!< The constraint type (e.g. BHP or SplinePoints positions).
            QString well; //!< The name of the well this Constraint applies to.
            QStringList wells; //!< List of well names if the constraint applies to more than one.
            double max; //!< Max limit when using constraints like BHP.
            double min; //!< Min limit when using constraints like BHP.
            double box_imin, box_imax, box_jmin, box_jmax, box_kmin, box_kmax; //!< Min max limits for geometrix box constraints.
            double max_length;
            double min_length;
            double min_distance;
            int max_iterations;
            ConstraintWellSplinePointsType spline_points_type; //!< How the SplinePoints constraint is given when SplinePoints constraint type is selected.
            QList<RealMaxMinLimit> spline_points_limits; //!< Box limits a spline point needs to be within to be valid when SplinePoints constraint type is selected.
        };

        OptimizerType type() const { return type_; } //!< Get the Optimizer type (e.g. Compass).
        OptimizerMode mode() const { return mode_; } //!< Get the optimizer mode (maximize/minimize).
        Parameters parameters() const { return parameters_; } //!< Get the optimizer parameters.
        Objective objective() const { return objective_; } //!< Get the optimizer objective function.
        QList<Constraint> constraints() const { return constraints_; } //!< Get the optimizer constraints.

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
