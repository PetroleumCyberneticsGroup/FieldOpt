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

#ifndef WELL_H
#define WELL_H

#include <QVector>
#include "tr1/memory"
#include "model/components/component.h"
#include "model/stream.h"
#include "model/wellcontrol.h"
#include "model/wellconnection.h"
#include "model/wellconnectionvariable.h"
#include "model/wellpath.h"
#include "model/cost.h"
#include "variables/intvariable.h"

using std::tr1::shared_ptr;

class WellConnection;
class WellConnectionVariable;
class WellPath;
class Stream;
class Cost;
class IntVariable;

/*!
 * \brief Base class for representation of wells.
 */
class Well : public Component
{
public:
    enum well_type {PROD, INJ}; //!< Enum containing the two different types of wells. PROD = Production. INJ = Injection.

private:
    Well::well_type m_type;               //!< Type of well (producer/injector). \todo This is marked as todo in the ResOpt code.
    QString m_name;                       //!< Name of this well. \todo This is marked as todo in the ResOpt code.
    QString m_group;                      //!< The group this well belongs to. \todo This is marked as todo in the ResOpt code.
    double m_bhp_limit;                   //!< The BHP limit when the well is on rate control.
    WellControl::control_type m_bhp_inj;  //!< The phase injected when the injector is on BHP control. \todo Should probably be moved to the InjectionWell class.

    QVector<WellConnection*> m_connections;              //!< Well perforations.
    QVector<WellConnectionVariable*> m_var_connections;  //!< Variable well perforations.
    WellPath* p_well_path;                               //!< Well path (MRST)
    QVector<WellControl*> m_schedule;                    //!< Control schedule for the well \todo This is marked as todo in the ResOpt code.

    Cost *p_cost;                            //!< The cost associated with installing the well (only used if the install time is set).
    shared_ptr<IntVariable> p_install_time;  //!< The installation time of the separator.

public:
    Well();               //!< Default constructor. Initialize the injector control type to QWAT, and cost and well path to 0.
    Well(const Well &w);  //!< Copy constructor.
    virtual ~Well();      //!< Default destructor.

    void initialize();                                                        //!< Initialize all the streams by adding one Stream for each schedule entry.
    bool isInstalled(int i);                                                  //!< Check whether the well is installed at schedule entry i.
    bool hasInstallTime() const {return p_install_time != 0;}                 //!<
    bool hasCost() const {return p_cost != 0;}                                //!<
    bool hasVariableConnections() const {return m_var_connections.size()>0;}  //!<
    bool hasWellPath() const {return p_well_path != 0;}                       //!<

    virtual Well* clone() const = 0;          //!<
    virtual void setAutomaticType() = 0;      //!<
    virtual QString description() const = 0;  //!<

    void addConnection(WellConnection *c) {m_connections.push_back(c);}  //!< Add a new connection (perforation) to the well.
    void addVariableConnection(WellConnectionVariable *vc);              //!< Add a new WellConnectionVariable to the well.
    void addControl(WellControl *c) {m_schedule.push_back(c);}           //!< Add a new control WellControl (schedule entry) to the well's schedule.
    void setType(Well::well_type t) {m_type = t;}                        //!< Set the type of well (injector or producer)
    void setName(const QString &n) {m_name = n;}                         //!< Set the Well name.
    void setGroup(const QString &g) {m_group = g;}                       //!< Set the group that the well belongs to.
    void setWellPath(WellPath *wp) {p_well_path = wp;}                   //!< Set the WellPath for this well. WellPath's are used by the MRST reservoir simulator.
    /*!
     * \brief Set the BHP (bottom hole pressure) limit for this Well.
     *
     * The BHP limit is the minimum allowed pressure for producers and the maximum allowed pressure for injectors.
     * The limit is only used when the well is on rate control, and is not considered a constraint.
     * \param p
     */
    void setBhpLimit(double p) {m_bhp_limit = p;}
    void setBhpInj(WellControl::contol_type t) {m_bhp_inj = t;}  //!< Set the default injection phase when injection well is on BHP control (BHP, QOIL, QGAS or QWAT)
    void setCost(Cost *c) {p_cost = c;}                                   //!< Set the cost of this well. \todo This is not currently implemented in the Cost class.
    void setInstallTime(shared_ptr<IntVariable> t) {p_install_time = t;}  //!< Set the install time of this well. \todo This is not currently implemented in the Cost class.


    Well::well_type type() const {return m_type;}                                                //!< Get the well type.
    QString typeAsString() const { return (m_type == Well::PROD ? "P" : "I"); }                  //!< Returns "P" if the well is a producer; otherwise "I".
    const QString& name() const {return m_name;}                                                 //!< Get the well name.
    const QString& group() const {return m_group;}                                               //!< Get the group that the well belongs to.
    double bhpLimit() const {return m_bhp_limit;}                                                //!< Get the BHP limit of the well.
    WellControl::contol_type bhpInj() const {return m_bhp_inj;}                                  //!< Get the injected phase when on BHP control.
    int numberOfConnections() const {return (m_connections.size() + m_var_connections.size());}  //!< Get the number of cells that the well is connected to (i.e. the number of perforations).
    int numberOfVariableConnections() const {return m_var_connections.size();}                   //!< Get the the number of variable connections.
    int numberOfConstantConnections() const {return m_connections.size();}                       //!< Get the number of constant connections.
    WellConnection* connection(int i) const;                                                     //!< Get the cell number of connection i.
    WellConnectionVariable* variableConnection(int i) const {return m_var_connections.at(i);}    //!< Get the variable connection at position i.
    WellConnection* constantConnection(int i) const {return m_connections.at(i);}                //!< Get the constant connection at position i.
    int numberOfControls() const {return m_schedule.size();}                                     //!< Get the number of well control set points (schedule entries) for this well.
    WellControl* control(int i) const {return m_schedule.at(i);}                                 //!< Get WellControl i from the schedule.
    Cost* cost() const {return p_cost;}                                                          //!< Get the cost of this well. \todo Cost of wells not yet implemented in the Cost class.
    shared_ptr<IntVariable> installTime() const {return p_install_time;}                         //!< Get the install time for the well. \todo Cost of wells not yet implemented in the Cost class.
    WellPath* wellPath() {return p_well_path;}                                                   //!< Get the WellPath associated with this Well.
};

#endif // WELL_H
