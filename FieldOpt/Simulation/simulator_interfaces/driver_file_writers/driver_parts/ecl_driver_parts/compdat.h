#ifndef COMPDAT_H
#define COMPDAT_H

#include "ecldriverpart.h"
#include "Model/wells/well.h"

namespace Simulation {
    namespace SimulatorInterfaces {
        namespace DriverFileWriters {
            namespace DriverParts {
                namespace ECLDriverParts {

/*!
 * \brief The Compdat class Generates the string for the WELSPECS section in the ECLIPSE simulator.
 *
 * The information is taken from both the Model::Wells::Well and Model::Wells::Wellbore::WellBlock classes.
 *
 * \todo It is likely that this object has to be deleted and recreated whenever the model changes to get the new settings.
 *
 * \todo Support WELSPECL.
 *
 * \todo Support ICDs.
 *
 * \todo Support spline-defined wells.
 */
                    class Compdat : public ECLDriverPart
                    {
                    public:
                        Compdat(QList<Model::Wells::Well *> *wells);
                        QString GetPartString();

                    private:
                        QList<QStringList> createWellEntries(Model::Wells::Well *well);
                        QStringList createBlockEntry(QString well_name, double wellbore_radius, Model::Wells::Wellbore::WellBlock *well_block);
                    };

                }
            }
        }
    }
}
#endif // COMPDAT_H
