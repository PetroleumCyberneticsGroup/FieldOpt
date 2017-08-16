/******************************************************************************
   Created by einar on 8/16/17.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include <iostream>
#include "pseudo_cont_vert.h"

namespace Model {
namespace Wells {
namespace Wellbore {
PseudoContVert::PseudoContVert(Settings::Model::Well well_settings,
                               Properties::VariablePropertyContainer *variable_container,
                               Reservoir::Grid::Grid *grid) {
    grid_ = grid;
    int i_idx = well_settings.pseudo_cont_position.i;
    int j_idx = well_settings.pseudo_cont_position.j;

    assert(i_idx >= 0 && i_idx < grid->Dimensions().nx);
    assert(j_idx >= 0 && j_idx < grid->Dimensions().ny);

    Reservoir::Grid::Cell init_cell;

    try {
        init_cell = grid_->GetCell(i_idx, j_idx, 0);
    } catch (std::exception const &e) {
        std::cerr << "Error: Could not initialize PseudoContVert for well " << well_settings.name.toStdString()
                  << ". Initial cell is outside grid" << std::endl;
        exit(1);
    }
    z_pos_ = init_cell.center().z();
    x_pos_ = new Properties::ContinousProperty(init_cell.center().x());
    y_pos_ = new Properties::ContinousProperty(init_cell.center().y());

    if (well_settings.pseudo_cont_position.is_variable) {
        x_pos_->setName("PseudoContVert#" + well_settings.name + "#x");
        y_pos_->setName("PseudoContVert#" + well_settings.name + "#y");
        variable_container->AddVariable(x_pos_);
        variable_container->AddVariable(y_pos_);
    }
}
WellBlock * PseudoContVert::GetWellBlock() {
    auto block = grid_->GetCellEnvelopingPoint(x_pos_->value(), y_pos_->value(), z_pos_);
    auto wb = new WellBlock(block.ijk_index().i(), block.ijk_index().j(), block.ijk_index().k());
    auto comp = new Completions::Perforation();
    comp->setTransmissibility_factor(-1.0);
    wb->AddCompletion(comp);
    return wb;
}

}
}
}
