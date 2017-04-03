/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "model.h"
#include <boost/lexical_cast.hpp>

namespace Model {

    Model::Model(Settings::Model settings)
    {
        grid_ = new Reservoir::Grid::ECLGrid(settings.reservoir().path.toStdString());
        variable_container_ = new Properties::VariablePropertyContainer();

        wells_ = new QList<Wells::Well *>();
        for (int well_nr = 0; well_nr < settings.wells().size(); ++well_nr) {
            wells_->append(new Wells::Well(settings, well_nr, variable_container_, grid_));
        }

        variable_container_->CheckVariableNameUniqueness();
    }

    void Model::ApplyCase(Optimization::Case *c)
    {
        for (QUuid key : c->binary_variables().keys()) {
            variable_container_->SetBinaryVariableValue(key, c->binary_variables()[key]);
        }
        for (QUuid key : c->integer_variables().keys()) {
            variable_container_->SetDiscreteVariableValue(key, c->integer_variables()[key]);
        }
        for (QUuid key : c->real_variables().keys()) {
            variable_container_->SetContinousVariableValue(key, c->real_variables()[key]);
        }
        for (Wells::Well *w : *wells_) {
            w->Update();
        }
        verify();
        current_case_id_ = c->id();
    }

    void Model::verify()
    {
        verifyWells();
    }

    void Model::verifyWells()
    {
        for (Wells::Well *well : *wells_) {
            verifyWellTrajectory(well);
        }
    }

    void Model::verifyWellTrajectory(Wells::Well *w)
    {
        for (Wells::Wellbore::WellBlock *wb : *w->trajectory()->GetWellBlocks()) {
            verifyWellBlock(wb);
        }
    }

    void Model::verifyWellBlock(Wells::Wellbore::WellBlock *wb)
    {
        if (wb->i() < 1 || wb->i() > grid()->Dimensions().nx ||
            wb->j() < 1 || wb->j() > grid()->Dimensions().ny ||
            wb->k() < 1 || wb->k() > grid()->Dimensions().nz)
            throw std::runtime_error("Invalid well block detected: ("
                                     + boost::lexical_cast<std::string>(wb->i()) + ", "
                                     + boost::lexical_cast<std::string>(wb->j()) + ", "
                                     + boost::lexical_cast<std::string>(wb->k()) + ")"
            );
    }

}

