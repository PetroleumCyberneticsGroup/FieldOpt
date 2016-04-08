#include "adgprs_wellcontrols.h"

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace AdgprsDriverParts {

WellControls::WellControls(QList<Model::Wells::Well *> *wells)
    : Simulation::SimulatorInterfaces::DriverFileWriters::DriverParts::ECLDriverParts::WellControls(wells)
{}

QString WellControls::GetPartString()
{
    return Simulation::SimulatorInterfaces::DriverFileWriters::DriverParts::ECLDriverParts::WellControls::GetPartString();
}

QString WellControls::createTimeEntry(int time)
{
    if (time == 0) return "";

    // Find prev time step
    int prev_step;
    for (int i = 0; i < time_entries_.keys().size(); ++i) {
        if (time_entries_.keys()[i] == time) {
            prev_step = time_entries_.keys()[i-1];
            break;
        }
    }
    int dt = time-prev_step;
    return QString("TSTEP\n\t%1/\n").arg(dt);
}


}}}}}
