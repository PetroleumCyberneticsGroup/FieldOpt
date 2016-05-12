/******************************************************************************
 *
 * optimizer.h
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

#ifndef SETTINGS_OPTIMIZER_H
#define SETTINGS_OPTIMIZER_H

#include "Utilities/settings/settings.h"

#include <QList>
#include <QString>
#include <QStringList>

namespace Utilities {
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
    enum OptimizerType { Compass };
    enum OptimizerMode { Maximize, Minimize };
    enum ConstraintType { BHP, Rate, SplinePoints,
                          WellSplineLength, WellSplineInterwellDistance,
                          CombinedWellSplineLengthInterwellDistance };
    enum ConstraintWellSplinePointsType { MaxMin, Function};
    enum ObjectiveType { WeightedSum };

    struct Parameters {
        int max_evaluations; //!< Maximum number of evaluations allowed before terminating the optimization run.
        double initial_step_length; //!< The initial step length in the algorithm when applicable.
        double minimum_step_length; //!< The minimum step length in the algorithm when applicable.
    };

    struct Objective {
        ObjectiveType type; //!< The objective definition type (e.g. WeightedSum)
        struct WeightedSumComponent {
            double coefficient; QString property; int time_step;
            bool is_well_prop; QString well; }; //!< A component of a weighted sum formulatied objective function
        QList<WeightedSumComponent> weighted_sum; //!< The expression for the Objective function formulated as a weighted sum
    };

    struct Constraint {
        QString name; //!< The unique name of the variable this constraint applies to.
        struct RealCoordinate { double x; double y; double z; }; //!< Used to express (x,y,z) coordinates.
        struct RealMaxMinLimit { RealCoordinate max; RealCoordinate min; }; //!< Used to define a box-shaped 3D area. Max and min each define a corner.
        ConstraintType type; //!< The constraint type (e.g. BHP or SplinePoints positions).
        QString well; //!< The name of the well this Constraint applies to.
        QStringList wells; //!< List of well names if the constraint applies to more than one.
        double max; //!< Max limit when using constraints like BHP.
        double min; //!< Min limit when using constraints like BHP.
        double max_length;
        double min_length;
        double min_distance;
        double max_iterations;
        ConstraintWellSplinePointsType spline_points_type; //!< How the SplinePoints constraint is given when SplinePoints constraint type is selected.
        QString well_spline_points_function; //!< The function a spline point needs to adhere to to be valid when SplinePoints constraint type is selected.
        QList<RealMaxMinLimit> spline_points_limits; //!< Box limits a spline point needs to be within to be valid when SplinePoints constraint type is selected.
    };

    OptimizerType type() const { return type_; } //!< Get the Optimizer type (e.g. Compass).
    OptimizerMode mode() const { return mode_; } //!< Get the optimizer mode (maximize/minimize).
    Parameters parameters() const { return parameters_; } //!< Get the optimizer parameters.
    Objective objective() const { return objective_; } //!< Get the optimizer objective function.
    QList<Constraint> *constraints() const { return constraints_; } //!< Get the optimizer constraints.

private:
    Optimizer(QJsonObject json_optimizer);
    OptimizerType type_;
    Parameters parameters_;
    Objective objective_;
    QList<Constraint> *constraints_;
    OptimizerMode mode_;
    Constraint parseSingleConstraint(QJsonObject json_constraint);
};

}
}

#endif // SETTINGS_OPTIMIZER_H
