#include "wellstre.h"

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace AdgprsDriverParts {

Wellstre::Wellstre(QList<Model::Wells::Well *> *wells, Settings::Simulator::SimulatorFluidModel fluid_model)
{
    wells_ = wells;
    fluid_model_ = fluid_model;
}

QString Wellstre::GetPartString()
{
    return createKeyword();
}

QString Wellstre::createKeyword()
{
    QString wellstre = "WELLSTRE\n";
    for (int i = 0; i < wells_->length(); ++i) {
        if (wells_->at(i)->type() == Settings::Model::WellType::Injector) {
            QString line = QString("\t%1 /\n").arg(createWellEntry(wells_->at(i)));
            wellstre.append(line);
        }
    }
    wellstre.append("/\n");
    return wellstre;
}

QString Wellstre::createWellEntry(Model::Wells::Well *well)
{
    if (well->type() != Settings::Model::WellType::Injector) // Return an empty string if the well is not an injector
        return "";

    if (fluid_model_ == Settings::Simulator::SimulatorFluidModel::BlackOil) {
        initializeBaseEntryLine(4);
        base_entry_line_[0] = well->name();
        base_entry_line_[1] = "0";
        base_entry_line_[2] = "0";
        base_entry_line_[3] = "0";

        switch (well->preferred_phase()) {
        case Settings::Model::PreferredPhase::Water:
            base_entry_line_[3] = "1";
            break;
        case Settings::Model::PreferredPhase::Gas:
            base_entry_line_[1] = "1";
            break;
        case Settings::Model::PreferredPhase::Oil:
            base_entry_line_[2] = "1";
            break;
        default:
            base_entry_line_[3] = "1";
            break;
        }
    }
    else { // If its not a black oil model, it must be a dead oil model
        initializeBaseEntryLine(3);
        base_entry_line_[0] = well->name();
        base_entry_line_[1] = "0";
        base_entry_line_[2] = "0";

        switch (well->preferred_phase()) {
        case Settings::Model::PreferredPhase::Water:
            base_entry_line_[1] = "1";
            break;
        case Settings::Model::PreferredPhase::Oil:
            base_entry_line_[2] = "1";
            break;
        default:
            base_entry_line_[1] = "1";
            break;
        }
    }

    return base_entry_line_.join("\t");
}

}}}}}
