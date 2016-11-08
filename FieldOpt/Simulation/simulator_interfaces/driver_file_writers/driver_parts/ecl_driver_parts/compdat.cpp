#include "compdat.h"
#include <iostream>

namespace Simulation {
    namespace SimulatorInterfaces {
        namespace DriverFileWriters {
            namespace DriverParts {
                namespace ECLDriverParts {

                    Compdat::Compdat(QList<Model::Wells::Well *> *wells)
                    {
                        initializeBaseEntryLine(13);
                        head_ = "COMPDAT";
                        foot_ = "/\n\n";
                        for (int i = 0; i < wells->size(); ++i) {
                            if (wells->at(i)->trajectory()->GetWellBlocks()->size() > 0)
                                entries_.append(createWellEntries(wells->at(i)));
                        }
                    }

                    QString Compdat::GetPartString()
                    {
                        QString entries = head_ + "\n";
                        for (QStringList entry : entries_) {
                            entries.append("    " + entry.join(" ") + " /\n");
                        }
                        entries.append("\n" + foot_);
                        return entries;
                    }

                    QList<QStringList> Compdat::createWellEntries(Model::Wells::Well *well)
                    {
                        QList<QStringList> block_entries = QList<QStringList>();
                        for (int i = 0; i < well->trajectory()->GetWellBlocks()->size(); ++i) {
                            block_entries.append(createBlockEntry(well->name(), well->wellbore_radius(), well->trajectory()->GetWellBlocks()->at(i)));
                        }
                        return block_entries;
                    }

                    QStringList Compdat::createBlockEntry(QString well_name, double wellbore_radius, Model::Wells::Wellbore::WellBlock *well_block)
                    {
                        QStringList block_entry = QStringList(base_entry_line_);
                        block_entry[0] = well_name;
                        block_entry[1] = QString::number(well_block->i());
                        block_entry[2] = QString::number(well_block->j());
                        block_entry[3] = QString::number(well_block->k());
                        block_entry[4] = QString::number(well_block->k());

                        if (well_block->HasPerforation()) {
                            block_entry[5] = "OPEN";
                            if (well_block->GetPerforation()->transmissibility_factor() >= 0.0)
                                block_entry[7] = QString::number(well_block->GetPerforation()->transmissibility_factor());
                            // WI is defaulted if a negative value is provided.
                        }
                        else {
                            block_entry[5] = "SHUT";
                        }

                        block_entry[8] = QString::number(wellbore_radius);

                        /// \note By default, this is set to X. This is primarily relevant if the well passes diagonally through a block.
                        block_entry[12] = "X";
                        switch (well_block->directionOfPenetration()) {
                            case Model::Wells::Wellbore::WellBlock::DirectionOfPenetration::X:
                                block_entry[12] = "X";
                                break;
                            case Model::Wells::Wellbore::WellBlock::DirectionOfPenetration::Y:
                                block_entry[12] = "Y";
                                break;
                            case Model::Wells::Wellbore::WellBlock::DirectionOfPenetration::Z:
                                block_entry[12] = "Z";
                                break;
                            case Model::Wells::Wellbore::WellBlock::DirectionOfPenetration::W:
                                std::runtime_error("The model library was unable to determine the direction of penetration.");
                            default:
                                std::runtime_error("The model library was unable to determine the direction of penetration.");
                        }
                        return block_entry;
                    }

                }
            }
        }
    }
}
