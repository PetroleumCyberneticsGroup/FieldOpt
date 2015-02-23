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
#include "variables/intvariable.h"

using std::tr1::shared_ptr;

class WellControl;
class WellConnection;
class WellConnectionVariable;
class WellPath;
class Cost;

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
    Well();
    ~Well();
};

#endif // WELL_H
