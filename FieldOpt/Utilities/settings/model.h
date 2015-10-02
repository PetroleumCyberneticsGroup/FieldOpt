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

#ifndef MODEL_H
#define MODEL_H

#include "settings.h"

#include <QString>
#include <QList>

namespace Utilities {
namespace Settings {

/*!
 * \brief The Model class contains model-specific settings. Model settings objects may _only_ be
 * created by the Settings class. They are created when reading a JSON-formatted "driver file".
 */
class Model
{
    friend class Settings;

public:
    enum ReservoirGridSourceType { ECLIPSE };
    enum WellType { Injector, Producer };
    enum WellControlType { BHPControl, RateControl };
    enum WellDefinitionType { WellBlocks, WellSpline };
    enum WellVariableType { BHP, Rate, SplinePoints };

    struct Reservoir {
        ReservoirGridSourceType type; //!< The source of the grid file (which reservoir simulator produced it).
        QString path; //!< Path to the reservoir grid file, e.g. a .EGRID or .GRID file produced by ECLIPSE.
    };

    struct Well {
        struct IntegerCoordinate { int i; int j; int k; };
        struct RealCoordinate { double x; double y; double z; };
        struct Variable {
            QString name; //!< A unique name for the variable.
            WellVariableType type; //!< The type of variable (what kind of property it applies to, _not_ int/float).
            QList<int> time_steps; //!< The time steps at which the variable is allowed to change value.
            QList<int> variable_spline_point_indices; //!< The indices of coordinates in the spline points list that are variable. The rest are taken as constant.
        };
        QString name; //!< The name to be used for the well.
        WellType type; //!< The well type, i.e. producer or injector.
        WellControlType control; //!< Whether the well is controlled by rate or bhp.
        IntegerCoordinate heel; //!< The heel of the well. Must _always_ be defined.
        WellDefinitionType definition_type; //!< How the well path is defined.
        double bhp; //!< BHP when using BHP control.
        double rate; //!< Rate when using rate control.
        QList<IntegerCoordinate> well_blocks; //!< Well blocks when the well path is defined by WellBlocks.
        QList<RealCoordinate> spline_points; //!< Spline points when the well path is defined by SplinePoints.
        QList<Variable> variables; //!< List of variables for the well (e.g. pressure, rate or spline point positions).
    };

    Reservoir reservoir() const { return reservoir_; } //!< Get the struct containing reservoir settings.
    QList<Well> wells() const { return wells_; } //!< Get the struct containing settings for the well(s) in the model.

private:
    Model(QJsonObject json_model);
    Reservoir reservoir_;
    QList<Well> wells_;

    void readReservoir(QJsonObject json_reservoir);
    Well readSingleWell(QJsonObject json_well);
    Well::Variable readSingleVariable(QJsonObject json_variable);

    bool variableNameExists(QString varialbe_name) const;
};

}
}

#endif // MODEL_H
