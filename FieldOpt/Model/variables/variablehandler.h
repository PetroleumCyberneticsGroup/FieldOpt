/******************************************************************************
 *
 *
 *
 * Created: 20.10.2015 2015 by einar
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

#ifndef VARIABLEHANDLER_H
#define VARIABLEHANDLER_H

#include "Utilities/settings/settings.h"

namespace Model {
namespace Variables {

/*!
 * \brief The VariableHandler class maintains a tree of all parts of the model that could possibly be
 * declared variable (in the sense that the value can change during optimization). It is used during
 * initialization to determine which values in the model should be added to the VariableContainer,
 * i.e. the lists of variables that are passed to the optimizer at run time.
 */
class VariableHandler
{
public:
    VariableHandler(Utilities::Settings::Model settings);

    /*!
     * \brief The Control class is an internal representation of a control within the variable handler.
     * The boolean fields indicate whether or not the property is variable at the given time.
     */
    class Control {
        friend class VariableHandler;
        friend class Well;
        Control(int time) {
            time_ = time;
        }
        int time_;
        bool open_ = false;
        bool rate_ = false;
        bool bhp_ = false;

    public:
        int time() const { return time_; }  //!< Get the time step for this control.
        bool open() const { return open_; } //!< Returns true if the well's open/closed state can vary at this time step, otherwise false.
        bool rate() const { return rate_; } //!< Returns true if the well's rate can vary at this time step, otherwise false.
        bool bhp() const { return bhp_; }   //!< Returns true if the well's BHP can vary at this time step, otherwise false.
    };

    class Perforation {
        friend class Well;
        Perforation(int id) { id_ = id; }
        int id_;
        int i_, j_, k_, transmissibility_factor_;

    public:
        bool transmissibility_factor() { return transmissibility_factor_; }
    };

    /*!
     * \brief The Well class is an internal representation of a well within the variable handler.
     * The well class primarily acts as a root node and container for Controls etc.
     */
    class Well {
        friend class VariableHandler;
        friend class Control;
        Well(QString name) {
            name_ = name;
            controls_ = QList<Control *>();
        }
        QString name_;
        QList<Control *> controls_;
    public:
        QString name() const { return name_; } //!< Get the name of the well.
    };

    /*!
     * \brief GetControl Get a given control for a well with a given name.
     * \param well_name The name of the well the control belongs to.
     * \param time The time step for the control.
     */
    Control *GetControl(QString well_name, int time);

    /*!
     * \brief GetWell Get the well with the specified name.
     * \param well_name The name of the well to get.
     */
    Well *GetWell(QString well_name);

private:
    QList<Well *> wells_;

    int wellControlIndex(Well *well, int time);
};

}
}

#endif // VARIABLEHANDLER_H
