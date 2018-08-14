#ifndef ADGPRS_WELLCONTROLS_H
#define ADGPRS_WELLCONTROLS_H

#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/wellcontrols.h"
#include "Model/wells/well.h"
#include <QList>

namespace Simulation {
namespace AdgprsDriverParts {

class WellControls : public ECLDriverParts::WellControls
{
public:
    WellControls(QList<Model::Wells::Well *> *wells, QList<int> control_times);


    // DriverPart interface
public:
    QString GetPartString();

private:


    // WellControls interface
private:
    QString createTimeEntry(int time, int prev_time);
};

}}

#endif // ADGPRS_WELLCONTROLS_H
