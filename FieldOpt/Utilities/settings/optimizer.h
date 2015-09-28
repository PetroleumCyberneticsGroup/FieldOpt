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

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <QList>
#include <QString>

namespace Utilities {
namespace Settings {

class Optimizer
{
public:
    Optimizer();

    enum SimulatorType { Compass };

    struct Parameters {
        int max_evaluations;
        double initial_step_length;
        double minimum_step_length;
    };

    struct Objective {
        enum ObjectiveType { Expression };
        ObjectiveType type;
        QString expression;
    };

    enum ConstraintType { BHP, SplinePoints };
    enum ConstraintWellSplinePointsType { MaxMin, Function};
    struct Constraint {
        struct RealCoordinate { double x; double y; double z; };
        struct RealMaxMinLimit { RealCoordinate max; RealCoordinate min; };
        ConstraintType type;
        QString well;
        double max;
        double min;
        ConstraintWellSplinePointsType spline_points_type;
        QString well_spline_points_function;
        QList<RealMaxMinLimit> spline_points_limits;
    };

private:
    SimulatorType type_;
    Parameters parameters_;
    Objective objective_;
    QList<Constraint> constraints_;
};

}
}

#endif // OPTIMIZER_H
