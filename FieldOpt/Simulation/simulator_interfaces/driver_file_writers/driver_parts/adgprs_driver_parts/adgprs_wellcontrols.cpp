#include "adgprs_wellcontrols.h"

namespace Simulation {
namespace AdgprsDriverParts {

WellControls::WellControls(QList<Model::Wells::Well *> *wells, QList<int> control_times)
    : Simulation::ECLDriverParts::WellControls(wells, control_times)
{}

QString WellControls::GetPartString()
{
    return Simulation::ECLDriverParts::WellControls::GetPartString();
}

QString WellControls::createTimeEntry(int time, int prev_time)
{
    if (time == 0) return "";

    // Find prev time step
    int prev_step = 0;
    for (int i = 0; i < time_entries_.keys().size(); ++i) {
        if (time_entries_.keys()[i] == time) {
            prev_step = time_entries_.keys()[i-1];
            break;
        }
    }
    int dt = time-prev_step;
    return QString("TSTEP\n\t%1/\n").arg(dt);
}


}}
