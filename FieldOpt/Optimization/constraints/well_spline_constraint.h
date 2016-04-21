#ifndef WELLSPLINECONSTRAINT_H
#define WELLSPLINECONSTRAINT_H

#include "Utilities/settings/optimizer.h"
#include "Model/properties/variable_property_container.h"

namespace Optimization { namespace Constraints {

class WellSplineConstraint
{
protected:
    WellSplineConstraint() {}

    struct Coord {
        QUuid x;
        QUuid y;
        QUuid z;
    };

    struct Well {
        Coord heel;
        Coord toe;
    };

    Well initializeWell(QList<QPair<QUuid, QString>> vars, QString well_name);
    Coord initializeCoord(QList<QPair<QUuid, QString>> point_vars);
};

}}

#endif // WELLSPLINECONSTRAINT_H
