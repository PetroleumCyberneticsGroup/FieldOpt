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

#include "trajectory.h"
#include "Model/wells/well_exceptions.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "Utilities/verbosity.h"
#include "Utilities/printer.hpp"
#include "polar_spline.h"
#include "k_locked_polar_spline.h"

namespace Model {
namespace Wells {
namespace Wellbore {

Trajectory::Trajectory(Settings::Model::Well well_settings,
                       Properties::VariablePropertyContainer *variable_container,
                       ::Reservoir::Grid::Grid *grid,
                       Reservoir::WellIndexCalculation::wicalc_rixx *wic
)
{
    well_blocks_ = new QList<WellBlock *>();
    well_spline_ = 0;
    pseudo_cont_vert_ = 0;
    definition_type_ = well_settings.definition_type;
    if (well_settings.definition_type == Settings::Model::WellDefinitionType::WellBlocks) {
        initializeWellBlocks(well_settings, variable_container);
        calculateDirectionOfPenetration();
    }
    else if (well_settings.definition_type == Settings::Model::WellDefinitionType::WellSpline) {
        if (well_settings.convert_well_blocks_to_spline) {
            convertWellBlocksToWellSpline(well_settings, grid);
        }
        well_spline_ = new WellSpline(well_settings, variable_container, grid, wic);
        well_blocks_ = well_spline_->GetWellBlocks();
        calculateDirectionOfPenetration();
        if (VERB_MOD >= 3) {
            printWellBlocks();
        }
    }
    else if (well_settings.definition_type == Settings::Model::WellDefinitionType::PolarSpline) {
        well_spline_ = new PolarSpline(well_settings, variable_container, grid, wic);
        well_blocks_ = well_spline_->GetWellBlocks();
        calculateDirectionOfPenetration();
        if (VERB_MOD >= 3) {
            printWellBlocks();
        }
    }
    else if (well_settings.definition_type == Settings::Model::WellDefinitionType::KLockedPolarSpline) {
        well_spline_ = new KLockedPolarSpline(well_settings, variable_container, grid, wic);
        well_blocks_ = well_spline_->GetWellBlocks();
        calculateDirectionOfPenetration();
        if (VERB_MOD >= 3) {
            printWellBlocks();
        }
    }
    else if (well_settings.definition_type == Settings::Model::WellDefinitionType::PseudoContVertical2D) {
        pseudo_cont_vert_ = new PseudoContVert(well_settings, variable_container, grid);
        well_blocks_->append(pseudo_cont_vert_->GetWellBlock());
        calculateDirectionOfPenetration();
    }
}

int Trajectory::GetTimeSpentInWic() const {
    if (well_spline_ != 0) {
        return well_spline_->GetTimeSpentInWIC();
    }
    else return 0;
}

WellBlock *Trajectory::GetWellBlock(int i, int j, int k)
{
    for (int idx = 0; idx < well_blocks_->size(); ++idx) {
        if (well_blocks_->at(idx)->i() == i && well_blocks_->at(idx)->j() == j && well_blocks_->at(idx)->k() == k)
            return well_blocks_->at(idx);
    }
    throw WellBlockNotFoundException(i, j, k);
}

QList<WellBlock *> *Trajectory::GetWellBlocks()
{
    return well_blocks_;
}

void Trajectory::UpdateWellBlocks()
{
    // \todo This is the source of a memory leak: old well blocks are not deleted. Fix it.
    if (well_spline_ != 0) {
        if (well_spline_->HasGridChanged() || well_spline_->HasSplineChanged()) {
            well_blocks_ = well_spline_->GetWellBlocks();
        }
        else {
            if (VERB_MOD >= 3) {
                Printer::info("The well spline has not changed; well indices will not be recomputed.");
            }
        }
    }
    else if (pseudo_cont_vert_ != 0) {
        well_blocks_ = new QList<WellBlock *>();
        well_blocks_->append(pseudo_cont_vert_->GetWellBlock());
    }
    calculateDirectionOfPenetration();
}

double Trajectory::GetLength() const {
    if (definition_type_ == Settings::Model::WellDefinitionType::WellSpline) {
        return GetSplineLength();
    }
    else { // Block-defined
        throw std::runtime_error("Lenth of block-defined wells not yet implemented.");
    }
}

void Trajectory::initializeWellBlocks(Settings::Model::Well well,
                                      Properties::VariablePropertyContainer *variable_container)
{
    QList<Settings::Model::Well::WellBlock> blocks = well.well_blocks;
    for (int i = 0; i < blocks.size(); ++i) {
        well_blocks_->append(new WellBlock(blocks[i].i, blocks[i].j, blocks[i].k));
        if (blocks[i].is_variable) {
            well_blocks_->last()->i_->setName(blocks[i].name + "#i");
            well_blocks_->last()->j_->setName(blocks[i].name + "#j");
            well_blocks_->last()->k_->setName(blocks[i].name + "#k");
            variable_container->AddVariable(well_blocks_->last()->i_);
            variable_container->AddVariable(well_blocks_->last()->j_);
            variable_container->AddVariable(well_blocks_->last()->k_);
        }
        if (blocks[i].has_completion)
            well_blocks_->last()->AddCompletion(new Completions::Perforation(blocks[i].completion, variable_container));
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
Settings::Model::WellDefinitionType Trajectory::GetDefinitionType() {
    return definition_type_;
}
void Trajectory::convertWellBlocksToWellSpline(Settings::Model::Well &well_settings, Reservoir::Grid::Grid *grid) {
    if (VERB_MOD >= 2) {
        Printer::ext_info("Convering well " + well_settings.name.toStdString() + " to spline.", "Model", "Trajectory");
    }
//    std::cout << "Input blocks:" << std::endl;
//    for (auto imported_block : well_settings.well_blocks) {
//        std::cout << imported_block.i << ",\t" << imported_block.j << ",\t" << imported_block.k << std::endl;
//    }
    QList<Settings::Model::Well::SplinePoint> points;
    for (int p = 0; p < well_settings.n_spline_points; ++p) {
        int srndg_block_idx = std::min(
            well_settings.well_blocks.size() - 1,
            int(p * std::ceil(well_settings.well_blocks.size() / (well_settings.n_spline_points-1)))
        );
        Reservoir::Grid::Cell srndg_cell;
        try {
            if (VERB_MOD >=3) {
                std::cout << "Selected for spline conversion block nr " << srndg_block_idx << std::endl;
            }
            Settings::Model::Well::WellBlock srndg_block = well_settings.well_blocks[srndg_block_idx];
            srndg_cell = grid->GetCell(srndg_block.i-1, srndg_block.j-1, srndg_block.k-1);
        }
        catch (std::runtime_error e) {
            Printer::ext_warn("Unable to get grid cell needed for spline conversion. Trying adjacent block.", "Model", "Trajectory");
//            std::cout << "WARNING: Unable to get grid cell needed for spline conversion: " << e.what()
//                      << " Trying adjacent block." << std::endl;
//            std::cout << "Selected for spline conversion block nr " << srndg_block_idx - 2 << std::endl;
            Settings::Model::Well::WellBlock srndg_block = well_settings.well_blocks[srndg_block_idx - 2];
            srndg_cell = grid->GetCell(srndg_block.i-1, srndg_block.j-1, srndg_block.k-1);
        }
        if (VERB_MOD >=3) {
            std::cout << "Corresponding block: " << srndg_cell.ijk_index().i() + 1
                      << ", " << srndg_cell.ijk_index().j() + 1
                      << ", " << srndg_cell.ijk_index().k() + 1
                      << " with center at " << srndg_cell.center().x()
                      << ", " << srndg_cell.center().y()
                      << ", " << srndg_cell.center().z() << std::endl;

        }
        Settings::Model::Well::SplinePoint new_point;
        new_point.name = "SplinePoint#" + well_settings.name + "#P" + QString::number(p+1);
        new_point.x = srndg_cell.center().x();
        new_point.y = srndg_cell.center().y();
        new_point.z = srndg_cell.center().z();

        if (well_settings.is_variable_spline) {
            new_point.is_variable = true;
        }
        else {
            new_point.is_variable = false;
        }

        points.push_back(new_point);
    }

    points.first().name = "SplinePoint#" + well_settings.name + "#heel";
    points.last().name = "SplinePoint#" + well_settings.name + "#toe";
    well_settings.spline_points = points;
    std::cout << "Done Converting well " << well_settings.name.toStdString() << " to spline." << std::endl;
}
WellBlock *Trajectory::GetWellBlockByMd(double md) const {
    assert(well_blocks_->size() > 0);
    if (md > GetLength()) {
        throw std::runtime_error("Attempting to get well block at MD grater than well length.");
    }
    for (auto block : *well_blocks_) {
        if (md <= block->getExitMd() && md >= block->getEntryMd()) {
            return block;
        }
    }
    throw std::runtime_error("Unable to get well block by MD.");
}
double Trajectory::GetEntryMd(const WellBlock *wb) const {
    return wb->getEntryMd();
}
double Trajectory::GetExitMd(const WellBlock *wb) const {
    return wb->getExitMd();
}
void Trajectory::printWellBlocks() {
    std::cout << "I,\tJ,\tK,\tINX,\tINY,\tINZ,\tOUTX,\tOUTY,\tOUTZ" << std::endl;
    for (auto wb : *well_blocks_) {
        std::stringstream entry;
        entry << wb->i() << ",\t" << wb->j() << ",\t" << wb->k() << ",\t";
        entry.precision(12);
        entry << std::scientific;
        entry << wb->getEntryPoint().x() << ",\t" << wb->getEntryPoint().y() << ",\t" << wb->getEntryPoint().z() << ",\t"
              << wb->getExitPoint().x() << ",\t" << wb->getExitPoint().y() << ",\t" << wb->getExitPoint().z()
              << std::endl;
        std::cout << entry.str();
    }
}
double Trajectory::GetSplineLength() const {
    return well_blocks_->back()->getExitMd();
}
std::vector<WellBlock *> Trajectory::GetWellBlocksByMdRange(double start_md, double end_md) const {
    std::vector<WellBlock *> affected_blocks;
    for (auto wb : *well_blocks_) {
        if (( start_md >= wb->getEntryMd() && start_md <= wb->getExitMd() ) || // Start md inside block
            (   end_md >= wb->getEntryMd() &&   end_md <= wb->getExitMd() ) || // End md inside block
            ( start_md <= wb->getEntryMd() &&   end_md >= wb->getExitMd() )) { // Block between start and end blocks
            affected_blocks.push_back(wb);
        }
    }
    return affected_blocks;
}

}
}
}
