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

#ifndef PRESSUREBOOSTER_H
#define PRESSUREBOOSTER_H

#include <tr1/memory>
#include <QVector>

#include "pipe.h"
#include "Library/model/cost.h"
#include "Library/variables/intvariable.h"
#include "Library/variables/realvariable.h"
#include "Library/constraints/constraint.h"
#include "Library/model/pipeconnection.h"

class PressureBooster : public Pipe
{
private:
    Cost *p_cost;                                               //!< The cost associated with installing the separator
    shared_ptr<IntVariable> p_install_time;                     //!< The installation time of the separator
    shared_ptr<RealVariable> p_pressure_change;                 //!< The pressure that the booster can increase
    shared_ptr<RealVariable> p_capacity;                        //!< The maximum capacity of the booster
    QVector<shared_ptr<Constraint> > m_capacity_constraints;    //!< Constraint on the maximum capacity of the booster
    PipeConnection *p_outlet_connection;                        //!< The outlet pipe that this separator feeds

public:
    PressureBooster();                          //!< Default constructor. Sets p_cost and p_outlet_connection to 0.
    PressureBooster(const PressureBooster &p);  //!< Copy constructor.
    virtual ~PressureBooster();                 //!< Default destructor. Deletes p_cost and p_outlet_connection.

    virtual Pipe* clone() {return new PressureBooster(*this);}  //!< Get a copy of this PressureBooster.
    virtual void calculateInletPressure();                      //!< Calculate the inlet pressure of the pressure booster.
    virtual QString description() const;                        //!< Get a description for the driver file.

    void setupCapacityConstraints(const QVector<double> &master_schedule);  //!< Set up the capacity constraints.
    void updateCapacityConstraints();                                       //!< Update the values of the capacity constraints.

    void setOutletConnection(PipeConnection *c) {p_outlet_connection = c;}         //!< Set outlet connection.
    void setCost(Cost *c) {p_cost = c;}                                            //!< Set cost.
    void setInstallTime(shared_ptr<IntVariable> t) {p_install_time = t;}           //!< Set install time.
    void setPressureVariable(shared_ptr<RealVariable> p) {p_pressure_change = p;}  //!< Set pressure change variable.
    void setCapacityVariable(shared_ptr<RealVariable> c) {p_capacity = c;}         //!< Set set capacity variable.

    PipeConnection* outletConnection() {return p_outlet_connection;}                         //!< Get outlet connection
    Cost* cost() {return p_cost;}                                                            //!< Get cost.
    shared_ptr<IntVariable> installTime() {return p_install_time;}                           //!< Get install time.
    shared_ptr<RealVariable> pressureVariable() {return p_pressure_change;}                  //!< Get pressure change variable/
    shared_ptr<RealVariable> capacityVariable() {return p_capacity;}                         //!< Get capacity variable.
    QVector<shared_ptr<Constraint> > capacityConstraints() {return m_capacity_constraints;}  //!< Get capacity constraints.
};

#endif // PRESSUREBOOSTER_H
