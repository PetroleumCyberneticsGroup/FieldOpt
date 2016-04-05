#include "wellstre.h"

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace AdgprsDriverParts {

Wellstre::Wellstre(QList<Model::Wells::Well *> *wells)
{
    wells_ = wells;
}

QString Wellstre::GetPartString()
{
    return createKeyword();
}

QString Wellstre::createKeyword()
{
    QString wellstre = "WELLSTRE\n";
    for (int i = 0; i < wells_->length(); ++i) {
        if (wells_->at(i)->type() == Utilities::Settings::Model::WellType::Injector) {
            QString line = QString("\t%1 /\n").arg(createWellEntry(wells_->at(i)));
            wellstre.append(line);
        }
    }
    wellstre.append("/\n");
    return wellstre;
}

QString Wellstre::createWellEntry(Model::Wells::Well *well)
{
    if (well->type() != Utilities::Settings::Model::WellType::Injector) // Return an empty string if the well is not an injector
        return "";

    initializeBaseEntryLine(4);
    base_entry_line_[0] = well->name();
    base_entry_line_[1] = "0";
    base_entry_line_[2] = "0";
    base_entry_line_[3] = "0";

    switch (well->prefered_phase()) {
    case Utilities::Settings::Model::PreferedPhase::Water:
        base_entry_line_[3] = "1";
        break;
    case Utilities::Settings::Model::PreferedPhase::Gas:
        base_entry_line_[1] = "1";
        break;
    case Utilities::Settings::Model::PreferedPhase::Oil:
        base_entry_line_[2] = "1";
        break;
    default:
        base_entry_line_[3] = "1";
        break;
    }
    return base_entry_line_.join("\t");
}

}}}}}
