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

#ifndef VARIABLE_PROPERTY_HANDLER_H
#define VARIABLE_PROPERTY_HANDLER_H

#include "Utilities/settings/settings.h"

namespace Model {
namespace Properties {

/*!
 * \brief The VariablePropertyHandler class maintains a tree of all properties in the model that could
 * possibly be declared variable (in the sense that the value can change during optimization).
 * It is used during initialization to determine which values in the model should be added to the
 * VariableContainer, i.e. the lists of variables that are passed to the optimizer at run time.
 */
class VariablePropertyHandler
{
public:
    VariablePropertyHandler(Utilities::Settings::Model settings);

    /*!
     * \brief The Control class is an internal representation of a control within the variable handler.
     * The boolean fields indicate whether or not the property is variable at the given time.
     */
    class Control {
        friend class VariablePropertyHandler;
        friend class Well;
        Control(int time) {
            time_ = time;
            variable_name_ = "";
        }
        int time_;
        bool open_ = false;
        bool rate_ = false;
        bool bhp_ = false;
        QString variable_name_;

    public:
        QString variable_name() const { return variable_name_; } //!< Get the name of the variable
        int time() const { return time_; }  //!< Get the time step for this control.
        bool open() const { return open_; } //!< Returns true if the well's open/closed state can vary at this time step, otherwise false.
        bool rate() const { return rate_; } //!< Returns true if the well's rate can vary at this time step, otherwise false.
        bool bhp() const { return bhp_; }   //!< Returns true if the well's BHP can vary at this time step, otherwise false.
    };

    /*!
     * \brief The WellBlock class Keeps track of which properties of a well block are variable.
     */
    class WellBlock {
        friend class VariablePropertyHandler;
        friend class Well;
        WellBlock(::Utilities::Settings::Model::WellBlock well_block) {
            id_ = well_block.id;
            variable_name_ = "";
            position_ = false;
        }
        QString variable_name_;
        int id_;
        bool position_;

    public:
        QString variable_name() const { return variable_name_; } //!< Get the name of the variable
        bool position() const { return position_; }
    };

    /*!
     * \brief The Perforation class keeps track of which properties of a perforation are variable.
     */
    class Perforation {
        friend class VariablePropertyHandler;
        friend class Well;
        Perforation(::Utilities::Settings::Model::Completion completion) {
            id_ = completion.id;
            block_ = completion.well_block;
            transmissibility_factor_ = false;
            variable_name_ = "";
        }
        int id_;
        ::Utilities::Settings::Model::WellBlock block_;
        bool transmissibility_factor_;
        QString variable_name_;

    public:
        QString variable_name() const { return variable_name_; } //!< Get the name of the variable
        bool transmissibility_factor() const { return transmissibility_factor_; }
    };


    class SplinePoint {
        friend class VariablePropertyHandler;
        friend class Well;
        SplinePoint(int index) {
            index_ = index;
        }
        int index_;
        bool variable_;
        QString variable_name_;
    public:
        bool variable() { return variable_; }
        QString variable_name() { return variable_name_; }
    };

    /*!
     * \brief The Well class is an internal representation of a well within the variable handler.
     * The well class primarily acts as a root node and container for Controls etc.
     */
    class Well {
        friend class VariablePropertyHandler;
        friend class Control;
        Well(QString name) {
            name_ = name;
            controls_ = QList<Control *>();
            perforations_ = QList<Perforation *>();
            spline_points_ = QList<SplinePoint *>();
        }
        QString name_;
        QList<Control *> controls_;
        QList<Perforation *> perforations_;
        QList<WellBlock *> well_blocks_;
        QList<SplinePoint *> spline_points_;
        Perforation *getPerforation(int id);
        Perforation *getPerforation(::Utilities::Settings::Model::IntegerCoordinate *block);
        WellBlock *getWellBlock(int id);
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
     * \brief GetPerforation Gets the perforation with a given id from a given well.
     * \param completion_id The unique ID of the perforation assigned when reading the driver file.
     */
    Perforation *GetPerforation(int completion_id);

    /*!
     * \brief GetSplinePoint Get the spline point with the specified index from the specified well.
     * \param well_name The name of the well the spline point belongs to.
     * \param point_index The index (in the list in the well settings) of the point to get.
     */
    SplinePoint *GetSplinePoint(QString well_name, int point_index);

    /*!
     * \brief GetWell Get the well with the specified name.
     * \param well_name The name of the well to get.
     */
    Well *GetWell(QString well_name);

    /*!
     * \brief GetWellBlock Gets the well block with the specified id.
     * \param well_block_id The id of the block to get.
     */
    WellBlock *GetWellBlock(int well_block_id);

private:
    QList<Well *> wells_;

    int wellControlIndex(Well *well, int time);
};

}
}

#endif // VARIABLE_PROPERTY_HANDLER_H
