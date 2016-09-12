#ifndef WELLSPLINECONSTRAINT_H
#define WELLSPLINECONSTRAINT_H

#include "Settings/optimizer.h"
#include "Model/properties/variable_property_container.h"

namespace Optimization {
    namespace Constraints {

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
             * \brief initializeWell Initialize the Well and Coord datastructures using the provided
             * variables.
             * \param vars the six variables defining the spline for a well.
             */
            Well initializeWell(QList<Model::Properties::ContinousProperty *> vars);

        };

    }
}

#endif // WELLSPLINECONSTRAINT_H
