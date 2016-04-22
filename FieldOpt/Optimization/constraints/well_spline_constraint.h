#ifndef WELLSPLINECONSTRAINT_H
#define WELLSPLINECONSTRAINT_H

#include "Utilities/settings/optimizer.h"
#include "Model/properties/variable_property_container.h"

namespace Optimization { namespace Constraints {

/*!
 * \brief The WellSplineConstraint class acts as a parent class for constraints dealing with well splines.
 *
 * This class provides datastructures with initialization methods that ease the application of well spline
 * constraints.
 */
class WellSplineConstraint
{
protected:
    WellSplineConstraint() {}

    /*!
     * \brief The Coord struct hodlds the UUIDs for the variables containing coordinate
     * values of a WellSpline point.
     */
    struct Coord {
        QUuid x;
        QUuid y;
        QUuid z;
    };

    /*!
     * \brief The Well struct Holds the heel and toe coordinates for a well defined by
     * a WellSpline.
     */
    struct Well {
        Coord heel;
        Coord toe;
    };

    /*!
     * \brief initializeWell Initialize the Well and Coord datastructures for a well with
     * the specified name, using the provided variables.
     * \param vars The variables containing the name for for the constraint.
     * \param well_name The name of the well the struct should be created for.
     * \return A Well struct for the well with the specified name, with Coord structs
     * representing its heel and toe.
     */
    Well initializeWell(QList<QPair<QUuid, QString>> vars, QString well_name);

    /*!
     * \brief initializeCoord Initialize a Coord struct.
     * \param point_vars The variables for a point (either heel or toe). Must _only_
     * contain the three variables the point should be created for.
     * \return A Coord struct representing the heel or toe of a well.
     */
    Coord initializeCoord(QList<QPair<QUuid, QString>> point_vars);
};

}}

#endif // WELLSPLINECONSTRAINT_H
