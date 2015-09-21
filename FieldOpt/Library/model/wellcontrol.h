/*
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einarjba@stud.ntnu.no>
 *
 * The code is largely based on ResOpt, created by  Aleksander O. Juell <aleksander.juell@ntnu.no>
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
 */

#ifndef WELLCONTROL_H
#define WELLCONTROL_H

#include <tr1/memory>
#include <QString>
#include "variables/realvariable.h"

using std::tr1::shared_ptr;
class RealVariable;

/*!
 * \brief Class containing information on one schedule entry for a well.
 *
 * This class holds information on one set point in the schedule for a Well, and is used to generate the input
 * schedule for the reservoir simulator. The time, type, and set point (rate or pressure) is stored.
 */
class WellControl
{
public:
    enum control_type {BHP, QOIL, QGAS, QWAT};  //!< The different types of control that may be applied to a well.

private:
    shared_ptr<RealVariable> p_control_var;  //!< Variable containing the current set point. \todo This is marked as todo in the ResOpt code.
    WellControl::control_type m_type;        //!< Type of well control. \todo This is marked as todo in the ResOpt code.
    double m_end_time;                       //!< Stop time. \todo This is marked as todo in the ResOpt code.

public:
    WellControl() {}                    //!< Default constructor. Does nothing.
    WellControl(const WellControl &c);  //!< Copy contructor.
    ~WellControl() {}                   //!< Default destructor. Does nothing.

    QString description();  //!< Return a string description of this object.

    void setControlVar(shared_ptr<RealVariable> v) { p_control_var = v; }  //!< Set the variable to be used for this schedule entry.
    void setType(WellControl::control_type t) { m_type = t; }              //!< Set the type of control to be used for this well at this time.
    void setEndTime(double t) { m_end_time = t; }                          //!< Set the end time of the schedule entry.

    shared_ptr<RealVariable> controlVar() { return p_control_var; }  //!< Return the Variable used for the set point in this schedule entry.
    WellControl::control_type type() const { return m_type; }        //!< Return the type of control.
    double endTime() const { return m_end_time; }                    //!< Return the end time.
};

#endif // WELLCONTROL_H
