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
#include <iostream>
#include <cmath>

namespace Model {
namespace Wells {
namespace Wellbore {

Trajectory::Trajectory(Utilities::Settings::Model::Well well_settings,
                       Properties::VariablePropertyContainer *variable_container,
                       Properties::VariablePropertyHandler *variable_handler,
                       ::Model::Reservoir::Reservoir *reservoir)
{
    well_blocks_ = new QList<WellBlock *>();
    if (well_settings.definition_type == Utilities::Settings::Model::WellDefinitionType::WellBlocks) {
        initializeWellBlocks(well_settings, variable_container, variable_handler);
        calculateDirectionOfPenetration();
        well_spline_ = 0;
    }
    else if (well_settings.definition_type == Utilities::Settings::Model::WellDefinitionType::WellSpline) {
        well_spline_ = new WellSpline(well_settings, variable_container, variable_handler, reservoir);
        well_blocks_ = well_spline_->GetWellBlocks();
        calculateDirectionOfPenetration();
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

void Trajectory::UpdateWellBlocks()
{
    if (well_spline_ != 0)
        well_blocks_ = well_spline_->GetWellBlocks();
    calculateDirectionOfPenetration();
}

void Trajectory::initializeWellBlocks(Utilities::Settings::Model::Well well,
                                      Properties::VariablePropertyContainer *variable_container,
                                      Properties::VariablePropertyHandler *variable_handler)
{
    QList<Utilities::Settings::Model::WellBlock> blocks = well.well_blocks;
    for (int i = 0; i < blocks.size(); ++i) {
        well_blocks_->append(new WellBlock(blocks[i].position.i, blocks[i].position.j, blocks[i].position.k, blocks[i].id));
        if (variable_handler->GetWellBlock(blocks[i].id)->position() == true) {
            QString base_var_name = variable_handler->GetWellBlock(blocks[i].id)->variable_name() + "#" + QString::number(i) + "#";
            well_blocks_->last()->i_->setName(base_var_name + "i");
            well_blocks_->last()->j_->setName(base_var_name + "j");
            well_blocks_->last()->k_->setName(base_var_name + "k");
            variable_container->AddVariable(well_blocks_->last()->i_);
            variable_container->AddVariable(well_blocks_->last()->j_);
            variable_container->AddVariable(well_blocks_->last()->k_);
        }
        Completions::Completion *completion = getCompletion(well.completions, blocks[i].position, variable_container, variable_handler);
        if (completion != nullptr)
            well_blocks_->last()->AddCompletion(completion);
    }
}

void Trajectory::calculateDirectionOfPenetration()
{
    if (well_blocks_->size() == 1) { // Assuming that the well is vertical if it only has one block
        well_blocks_->first()->setDirectionOfPenetration(WellBlock::DirectionOfPenetration::Z);
        return;
    }
    // All but the last block use forward direction
    for (int i = 0; i < well_blocks_->size()-1; ++i) {
        if (     std::abs(well_blocks_->at(i)->i() - well_blocks_->at(i+1)->i()) == 1 &&
                 std::abs(well_blocks_->at(i)->j() - well_blocks_->at(i+1)->j()) == 0 &&
                 std::abs(well_blocks_->at(i)->k() - well_blocks_->at(i+1)->k()) == 0)
            well_blocks_->at(i)->setDirectionOfPenetration(WellBlock::DirectionOfPenetration::X);
        else if (std::abs(well_blocks_->at(i)->i() - well_blocks_->at(i+1)->i()) == 0 &&
                 std::abs(well_blocks_->at(i)->j() - well_blocks_->at(i+1)->j()) == 1 &&
                 std::abs(well_blocks_->at(i)->k() - well_blocks_->at(i+1)->k()) == 0)
            well_blocks_->at(i)->setDirectionOfPenetration(WellBlock::DirectionOfPenetration::Y);
        else if (std::abs(well_blocks_->at(i)->i() - well_blocks_->at(i+1)->i()) == 0 &&
                 std::abs(well_blocks_->at(i)->j() - well_blocks_->at(i+1)->j()) == 0 &&
                 std::abs(well_blocks_->at(i)->k() - well_blocks_->at(i+1)->k()) == 1)
            well_blocks_->at(i)->setDirectionOfPenetration(WellBlock::DirectionOfPenetration::Z);
        else
            well_blocks_->at(i)->setDirectionOfPenetration(WellBlock::DirectionOfPenetration::W);
    }

    // Last block uses backward direction
    if (     std::abs(well_blocks_->last()->i() - well_blocks_->at(well_blocks_->size()-2)->i()) == 1 &&
             std::abs(well_blocks_->last()->j() - well_blocks_->at(well_blocks_->size()-2)->j()) == 0 &&
             std::abs(well_blocks_->last()->k() - well_blocks_->at(well_blocks_->size()-2)->k()) == 0)
        well_blocks_->last()->setDirectionOfPenetration(WellBlock::DirectionOfPenetration::X);
    else if (std::abs(well_blocks_->last()->i() - well_blocks_->at(well_blocks_->size()-2)->i()) == 0 &&
             std::abs(well_blocks_->last()->j() - well_blocks_->at(well_blocks_->size()-2)->j()) == 1 &&
             std::abs(well_blocks_->last()->k() - well_blocks_->at(well_blocks_->size()-2)->k()) == 0)
        well_blocks_->last()->setDirectionOfPenetration(WellBlock::DirectionOfPenetration::Y);
    else if (std::abs(well_blocks_->last()->i() - well_blocks_->at(well_blocks_->size()-2)->i()) == 0 &&
             std::abs(well_blocks_->last()->j() - well_blocks_->at(well_blocks_->size()-2)->j()) == 0 &&
             std::abs(well_blocks_->last()->k() - well_blocks_->at(well_blocks_->size()-2)->k()) == 1)
        well_blocks_->last()->setDirectionOfPenetration(WellBlock::DirectionOfPenetration::Z);
    else
        well_blocks_->last()->setDirectionOfPenetration(WellBlock::DirectionOfPenetration::W);
}

Completions::Completion *Trajectory::getCompletion(QList<Utilities::Settings::Model::Completion> completions,
                                                   Utilities::Settings::Model::IntegerCoordinate block,
                                                   Properties::VariablePropertyContainer *variable_container,
                                                   Properties::VariablePropertyHandler *variable_handler)
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
