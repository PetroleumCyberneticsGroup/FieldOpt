/******************************************************************************
 *
 * trajectory.cpp
 *
 * Created: 24.09.2015 2015 by einar
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

#include "trajectory.h"
#include "Model/wells/well_exceptions.h"
#include "iostream"

namespace Model {
namespace Wells {
namespace Wellbore {

Trajectory::Trajectory(Utilities::Settings::Model::Well well_settings,
                       Variables::VariableContainer *variable_container,
                       Variables::VariableHandler *variable_handler)
{
    if (well_settings.definition_type == Utilities::Settings::Model::WellDefinitionType::WellBlocks) {
        well_blocks_ = new QList<WellBlock *>();
        initializeWellBlocks(well_settings, variable_container, variable_handler);
    }
    else if (well_settings.definition_type == Utilities::Settings::Model::WellDefinitionType::WellSpline) {
        std::cerr << "Spline definition of wells are not yet supported. Skipping the well." << std::endl;
    }
}

WellBlock *Trajectory::GetWellBlock(int i, int j, int k)
{
    for (int idx = 0; idx < well_blocks_->size(); ++idx) {
        if (well_blocks_->at(idx)->i() == i && well_blocks_->at(idx)->j() == j && well_blocks_->at(idx)->k() == k)
            return well_blocks_->at(idx);
    }
    throw WellBlockNotFoundException(i, j, k);
}

WellBlock *Trajectory::GetWellBlock(int id)
{
    for (int idx = 0; idx < well_blocks_->size(); ++idx) {
        if (well_blocks_->at(idx)->id() == id)
            return well_blocks_->at(idx);
    }
    throw WellBlockNotFoundException(id);
}

QList<WellBlock *> *Trajectory::GetWellBlocks()
{
    return well_blocks_;
}

void Trajectory::initializeWellBlocks(Utilities::Settings::Model::Well well,
                                      Variables::VariableContainer *variable_container,
                                      Variables::VariableHandler *variable_handler)
{
    QList<Utilities::Settings::Model::WellBlock> blocks = well.well_blocks;
    for (int i = 0; i < blocks.size(); ++i) {
        well_blocks_->append(new WellBlock(blocks[i].position.i, blocks[i].position.j, blocks[i].position.k, blocks[i].id));
        if (variable_handler->GetWellBlock(blocks[i].id)->position() == true) {
            variable_container->AddVariable(well_blocks_->last()->i_);
            variable_container->AddVariable(well_blocks_->last()->j_);
            variable_container->AddVariable(well_blocks_->last()->k_);
        }
        Completions::Completion *completion = getCompletion(well.completions, blocks[i].position, variable_container, variable_handler);
        if (completion != nullptr)
            well_blocks_->last()->AddCompletion(completion);
    }
}

Completions::Completion *Trajectory::getCompletion(QList<Utilities::Settings::Model::Completion> completions,
                                                   Utilities::Settings::Model::IntegerCoordinate block,
                                                   Variables::VariableContainer *variable_container,
                                                   Variables::VariableHandler *variable_handler)
{
    for (int i = 0; i < completions.size(); ++i) { // Look for a completion belonging to the block.
        if (completions[i].well_block.position.Equals(&block)) { // Found a block.
            if (completions[i].type == Utilities::Settings::Model::WellCompletionType::Perforation) // Initializing a perforation
                return new Completions::Perforation(completions[i], variable_container, variable_handler);
        }
    }
    return nullptr; // Found no matching completion
}

}
}
}
