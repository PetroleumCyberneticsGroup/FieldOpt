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

#include <QString>
#include <QList>

namespace Utilities {
namespace Settings {

class Model
{
public:
    Model();

    enum ReservoirGridSourceType { ECLIPSE };
    struct Reservoir {
        ReservoirGridSourceType type;
        QString path;
    };

    enum WellType { Injector, Producer };
    enum WellControlType { BHP, Rate };
    enum WellDefinitionType { WellBlocks, WellSpline };
    struct Well {
        struct IntegerCoordinate { int i; int j; int k; };
        struct RealCoordinate { double x; double y; double z; };
        struct Variable {
            enum Type { BHP, SplinePoints };
            QList<int> time_steps;
            QList<int> variable_spline_point_indices;
        };

        QString name;
        WellType type;
        WellControlType control;
        WellDefinitionType definition_type;
        double bhp;
        double rate;
        IntegerCoordinate heel;
        QList<IntegerCoordinate> well_blocks;
        QList<RealCoordinate> spline_points;
    };

private:
    Reservoir type_;
    QList<Well> wells_;
};

}
}

#endif // MODEL_H
