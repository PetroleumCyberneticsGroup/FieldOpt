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

#include "Utilities/settings/settings.h"

#include <QString>
#include <QList>
#include <QJsonArray>
#include <assert.h>

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
    enum ReservoirGridSourceType : int { ECLIPSE=1 };
    enum WellType : int { Injector=011, Producer=12 };
    enum ControlMode : int { BHPControl=21, RateControl=22 };
    enum InjectionType : int { WaterInjection=31, GasInjection=32 };
    enum WellDefinitionType : int { WellBlocks=41, WellSpline=42 };
    enum WellVariableType : int { BHP=51, Rate=52, SplinePoints=53, OpenShut=54, Transmissibility=55 };
    enum WellCompletionType : int { Perforation=61 };
    enum WellState : int { WellOpen=71, WellShut=72 };
    enum PreferedPhase : int { Oil=81, Water=82, Gas=83, Liquid=84 };
    enum Direction : int { X=91, Y=92, Z=93 };

    struct IntegerCoordinate {
        IntegerCoordinate() {}
        IntegerCoordinate(QJsonArray array) {
            assert(array.size() == 3);
            i = array[0].toInt(); j = array[1].toInt(); k = array[2].toInt();
        }
        bool Equals(const IntegerCoordinate *other) const {
            if (this->i == other->i && this->j == other->j && this->k == other->k) return true;
            else return false;
        }
        int i, j ,k;
    };
    struct RealCoordinate {
        RealCoordinate() {}
        RealCoordinate(QJsonArray array) {
            assert(array.size() == 3);
            x = array[0].toDouble(); y = array[1].toDouble(); z = array[2].toDouble();
        }
        double x, y, z;
    };

    struct ControlEntry {
        int time_step; //!< The time step this control is to be applied at.
        WellState state; //!< Whether the well is open or shut.
        ControlMode control_mode; //!< Control mode.
        double bhp; //!< Bhp target when well is on bhp control.
        double rate; //!< Rate target when well is on rate control.
        InjectionType injection_type; //!< Injector type (water/gas)
    };
    struct Completion {
        int id; //!< Unique numerical ID for the completion.
        WellCompletionType type; //!< Which type of completion this is (Perforation/ICD)
        IntegerCoordinate well_block; //!< The well block to which this completion belongs.
        double transmissibility_factor; //!< The transmissibility factor for this completion (used for perforations)
    };

    struct Reservoir {
        ReservoirGridSourceType type; //!< The source of the grid file (which reservoir simulator produced it).
        QString path; //!< Path to the reservoir grid file, e.g. a .EGRID or .GRID file produced by ECLIPSE.
    };

    struct Well {
        struct Variable {
            int parent_completion_id; //!< The id of the parent completion. Automatically assigned.
            QString name; //!< A unique name for the variable.
            WellVariableType type; //!< The type of variable (what kind of property it applies to, _not_ int/float).
            QList<int> time_steps; //!< The time steps at which the variable is allowed to change value.
            QList<IntegerCoordinate> blocks; //!< The blocks this variable should apply to
            QList<int> variable_spline_point_indices; //!< The indices of coordinates in the spline points list that are variable. The rest are taken as constant.
        };
        PreferedPhase prefered_phase; //!< The prefered phase for the well
        QString name; //!< The name to be used for the well.
        WellType type; //!< The well type, i.e. producer or injector.
        double wellbore_radius; //!< The wellbore radius
        Direction direction; //!< Direction of penetration
        IntegerCoordinate heel; //!< The heel of the well. Must _always_ be defined.
        WellDefinitionType definition_type; //!< How the well path is defined.
        QList<Completion> completions; //!< Well completions, i.e. perforations and ICDs.
        QList<IntegerCoordinate> well_blocks; //!< Well blocks when the well path is defined by WellBlocks.
        QList<RealCoordinate> spline_points; //!< Spline points when the well path is defined by SplinePoints.
        QList<Variable> variables; //!< List of variables for the well (e.g. pressure, rate or spline point positions).
        QList<ControlEntry> controls; //!< List of well controls
    };

    Reservoir reservoir() const { return reservoir_; } //!< Get the struct containing reservoir settings.
    QList<Well> wells() const { return wells_; } //!< Get the struct containing settings for the well(s) in the model.
    QList<int> control_times() const { return control_times_; } //!< Get the control times for the schedule

private:
    Model(QJsonObject json_model);
    Reservoir reservoir_;
    QList<Well> wells_;
    QList<int> control_times_;

    int next_completion_id;

    void readReservoir(QJsonObject json_reservoir);
    Well readSingleWell(QJsonObject json_well);
    Well::Variable readSingleVariable(QJsonObject json_variable, Well well);

    bool wellContainsBlock(Well well, IntegerCoordinate block);
    bool variableNameExists(QString varialbe_name) const;
    bool controlTimeIsDeclared(int time) const;
};

}
}

#endif // SETTINGS_MODEL_H
