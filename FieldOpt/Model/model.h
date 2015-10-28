/******************************************************************************
 *
 * model.h
 *
 * Created: 22.09.2015 2015 by einar
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

#include "reservoir/reservoir.h"
#include "variables/variablecontainer.h"
#include "schedule/schedule.h"
#include "wells/well.h"
#include "variables/variablehandler.h"
#include "Utilities/settings/model.h"

namespace Model {

class Model
{
public:
    Model(::Utilities::Settings::Model settings);

    Reservoir::Reservoir reservoir() const { return reservoir_; }
    Schedule::Schedule schedule() const { return schedule_; }
    Variables::VariableContainer *variables() const { return variables_; }
    QList<Wells::Well *> wells() const { return wells_; }

private:
    Reservoir::Reservoir reservoir_;
    Schedule::Schedule schedule_;
    Variables::VariableContainer *variables_;
    Variables::VariableHandler *variable_handler;

    QList<Wells::Well *> wells_;
};

}

#endif // MODEL_H
