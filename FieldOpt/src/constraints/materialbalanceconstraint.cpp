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

#include "materialbalanceconstraint.h"

MaterialBalanceConstraint::MaterialBalanceConstraint()
    : p_irv(0)
{
    p_con_oil = shared_ptr<Constraint>(new Constraint(0,0,0));
    p_con_gas = shared_ptr<Constraint>(new Constraint(0,0,0));
    p_con_water = shared_ptr<Constraint>(new Constraint(0,0,0));
}


void MaterialBalanceConstraint::emptyStream()
{
    m_stream.setGasRate(0.0);
    m_stream.setOilRate(0.0);
    m_stream.setWaterRate(0.0);
}


void MaterialBalanceConstraint::setInputRateVariable(InputRateVariable *irv)
{
    p_irv = irv;

    // Update the constraint names
    Pipe *p = irv->pipe();

    if(p != 0)
    {
        p_con_oil->setName("Material balance constraint for oil entering Pipe #" + QString::number(p->number()) + ", time = " + QString::number(p_irv->stream()->time()));
        p_con_gas->setName("Material balance constraint for gas entering Pipe #" + QString::number(p->number()) + ", time = " + QString::number(p_irv->stream()->time()));
        p_con_water->setName("Material balance constraint for water entering Pipe #" + QString::number(p->number()) + ", time = " + QString::number(p_irv->stream()->time()));
    }
}


bool MaterialBalanceConstraint::updateConstraints()
{
    if(p_irv == 0)
        return false;

    p_con_oil->setValue(m_stream.oilRate(true) - p_irv->oilVariable()->value());
    p_con_gas->setValue(m_stream.gasRate(true) - p_irv->gasVariable()->value());
    p_con_water->setValue(m_stream.waterRate(true) - p_irv->waterVariable()->value());

    return true;

}
