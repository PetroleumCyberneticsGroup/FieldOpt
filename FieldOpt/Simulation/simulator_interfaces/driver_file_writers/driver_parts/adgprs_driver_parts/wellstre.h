#ifndef WELLSTRE_H
#define WELLSTRE_H

#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/ecldriverpart.h"
#include "Model/wells/well.h"
#include "Utilities/settings/simulator.h"
#include <QStringList>


namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace AdgprsDriverParts {

class Wellstre : public ECLDriverParts::ECLDriverPart
{
public:
    Wellstre(QList<Model::Wells::Well *> *wells, Utilities::Settings::Simulator::SimulatorFluidModel fluid_model);

    // DriverPart interface
public:
    QString GetPartString();

private:
    QString createKeyword();
    QString createWellEntry(Model::Wells::Well *well);
    QList<Model::Wells::Well *> *wells_;
    Utilities::Settings::Simulator::SimulatorFluidModel fluid_model_;
};

}}}}}

#endif // WELLSTRE_H
