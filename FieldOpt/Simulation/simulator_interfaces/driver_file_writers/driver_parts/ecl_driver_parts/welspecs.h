#ifndef WELSPECS_H
#define WELSPECS_H

#include "ecldriverpart.h"
#include "Model/wells/well.h"
#include <QList>

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {

/*!
 * \brief The Welspecs class Generates the string for the
 * WELSPECS section in the ECLIPSE simulator.
 *
 * The information is taken from the Model::Wells::Well class.
 *
 * \todo It is likely that this object has to be deleted and
 * recreated whenever the model changes to get the new settings.
 * \todo Support WELSPECL.
 *
 * \todo This class should be less complicated. Review the model
 * and try to accomodate the wanted kind of access.
 */

class Welspecs : public ECLDriverPart
{
 public:
  Welspecs(QList<Model::Wells::Well *> *wells);
  QString GetPartString();

 private:
  QStringList createWellEntry(::Model::Wells::Well *well);
};

}
}
}
}
}

#endif // WELSPECS_H
