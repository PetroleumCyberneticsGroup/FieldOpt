#include "trajectory.h"
#include "Model/wells/well_exceptions.h"
#include <iostream>

namespace Model {
    namespace Wells {
        namespace Wellbore {

            Trajectory::Trajectory(Utilities::Settings::Model::Well well_settings,
                                   Properties::VariablePropertyContainer *variable_container,
                                   ::Reservoir::Reservoir *reservoir)
            {
                well_blocks_ = new QList<WellBlock *>();
                if (well_settings.definition_type == Utilities::Settings::Model::WellDefinitionType::WellBlocks) {
                    initializeWellBlocks(well_settings, variable_container);
                    calculateDirectionOfPenetration();
                    well_spline_ = 0;
                }
                else if (well_settings.definition_type == Utilities::Settings::Model::WellDefinitionType::WellSpline) {
                    well_spline_ = new WellSpline(well_settings, variable_container, reservoir);
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

            QList<WellBlock *> *Trajectory::GetWellBlocks()
            {
                return well_blocks_;
            }

            void Trajectory::UpdateWellBlocks()
            {
                if (well_spline_ != 0) {
                    // Delete old objects
                    for (int i = 0; i < well_blocks_->length(); ++i) {
                        delete well_blocks_->at(i);
                    }
                    well_blocks_ = well_spline_->GetWellBlocks();
                }
                calculateDirectionOfPenetration();
            }

            void Trajectory::initializeWellBlocks(Utilities::Settings::Model::Well well,
                                                  Properties::VariablePropertyContainer *variable_container)
            {
                QList<Utilities::Settings::Model::Well::WellBlock> blocks = well.well_blocks;
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

        }
    }
}
