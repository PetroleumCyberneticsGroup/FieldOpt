/******************************************************************************
 *
 * model.cpp
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

#include "model.h"

namespace Model {

Model::Model(Utilities::Settings::Model settings)
{
    reservoir_ = new Reservoir::Reservoir(settings.reservoir());
    variable_container_ = new Properties::VariablePropertyContainer();
    variable_handler_ = new Properties::VariablePropertyHandler(settings);

    wells_ = new QList<Wells::Well *>();
    for (int well_nr = 0; well_nr < settings.wells().size(); ++well_nr) {
        wells_->append(new Wells::Well(settings, well_nr, variable_container_, variable_handler_, reservoir_));
    }
}

void Model::ApplyCase(Optimization::Case *c)
{
    foreach (QUuid key, c->binary_variables().keys()) {
        variable_container_->SetBinaryVariableValue(key, c->binary_variables()[key]);
    }
    foreach (QUuid key, c->integer_variables().keys()) {
        variable_container_->SetDiscreteVariableValue(key, c->integer_variables()[key]);
    }
    foreach (QUuid key, c->real_variables().keys()) {
        variable_container_->SetContinousVariableValue(key, c->real_variables()[key]);
    }
    foreach (Wells::Well *w, *wells_) {
        w->trajectory()->UpdateWellBlocks();
    }
    verify();
}

void Model::verify()
{
    verifyWells();
}

void Model::verifyWells()
{
    foreach (Wells::Well *well, *wells_) {
        verifyWellTrajectory(well);
    }
}

void Model::verifyWellTrajectory(Wells::Well *w)
{
    foreach (Wells::Wellbore::WellBlock *wb, *w->trajectory()->GetWellBlocks()) {
        verifyWellBlock(wb);
    }
}

void Model::verifyWellBlock(Wells::Wellbore::WellBlock *wb)
{
    if (wb->i() < 1 || wb->i() > reservoir()->grid()->Dimensions().nx ||
            wb->j() < 1 || wb->j() > reservoir()->grid()->Dimensions().ny ||
            wb->k() < 1 || wb->k() > reservoir()->grid()->Dimensions().nz)
        throw std::runtime_error("Invalid well block detected: ("
                                 + std::to_string(wb->i()) + ", "
                                 + std::to_string(wb->j()) + ", "
                                 + std::to_string(wb->k()) + ")"
                                 );
}

}

