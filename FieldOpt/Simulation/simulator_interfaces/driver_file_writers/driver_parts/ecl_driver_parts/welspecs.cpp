#include "welspecs.h"
#include <iostream>

namespace Simulation {
    namespace SimulatorInterfaces {
        namespace DriverFileWriters {
            namespace DriverParts {
                namespace ECLDriverParts {

                    Welspecs::Welspecs(QList<Model::Wells::Well *> *wells)
                    {
                        initializeBaseEntryLine(10);
                        head_ = "WELSPECS";
                        foot_ = "/\n\n";
                        for (int i = 0; i < wells->size(); ++i) {
                            if (wells->at(i)->trajectory()->GetWellBlocks()->size() > 0)
                                entries_.append(createWellEntry(wells->at(i)));
                        }
                    }

                    QString Welspecs::GetPartString()
                    {
                        QString entries = head_ + "\n";
                        for (QStringList entry : entries_) {
                            entries.append("    " + entry.join(" ") + " /\n");
                        }
                        entries.append("\n" + foot_);
                        return entries;
                    }

                    QStringList Welspecs::createWellEntry(Model::Wells::Well *well)
                    {
                        QStringList entry = QStringList(base_entry_line_);
                        entry[0] = well->name();
                        if (well->group().length() >= 1)
                            entry[1] = well->group();
                        entry[2] = QString::number(well->heel_i());
                        entry[3] = QString::number(well->heel_j());

                        switch (well->preferred_phase()) {
                            case Settings::Model::PreferredPhase::Oil:
                                entry[5] = "OIL";
                                break;
                            case Settings::Model::PreferredPhase::Water:
                                entry[5] = "WATER";
                                break;
                            case Settings::Model::PreferredPhase::Gas:
                                entry[5] = "GAS";
                                break;
                            case Settings::Model::PreferredPhase::Liquid:
                                entry[5] = "LIQ";
                                break;
                            default:
                                throw std::runtime_error("The preferred phase was not recognized for well " + well->name().toStdString());
                        }
                        return entry;
                    }


                }
            }
        }
    }
}
