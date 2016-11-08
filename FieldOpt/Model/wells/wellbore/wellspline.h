#ifndef WELLSPLINE_H
#define WELLSPLINE_H

#include "trajectory.h"
#include "Reservoir/grid/eclgrid.h"
#include "Model/wells/wellbore/wellblock.h"
#include "FieldOpt-WellIndexCalculator/wellindexcalculator.h"
#include <QList>

namespace Model {
    namespace Wells {
        namespace Wellbore {

/*!
 * \brief The WellSpline class Generates the well blocks making up the trajectory from a set of spline points.
 * It uses the WellIndexCalculation library to do this.
 */
            class WellSpline
            {
            public:
                WellSpline(::Settings::Model::Well well_settings,
                           Properties::VariablePropertyContainer *variable_container,
                           Reservoir::Grid::Grid *grid);

                /*!
                 * \brief GetWellBlocks Get the set of well blocks with proper WI's defined by the spline.
                 * \return
                 */
                QList<WellBlock *> *GetWellBlocks();

            private:
                Reservoir::Grid::Grid *grid_;
                Settings::Model::Well well_settings_;

                Model::Properties::ContinousProperty *heel_x_;
                Model::Properties::ContinousProperty *heel_y_;
                Model::Properties::ContinousProperty *heel_z_;
                Model::Properties::ContinousProperty *toe_x_;
                Model::Properties::ContinousProperty *toe_y_;
                Model::Properties::ContinousProperty *toe_z_;

                /*!
                 * \brief getWellBlock Convert the BlockData returned by the WIC to a WellBlock with a Perforation.
                 * \note The IJK indexes are incremented by on to account for the zero-inclusive indices used by
                 * the ERT library. This is necessary because ECL and ADGPRS both use zero-exclusive indices.
                 * \param block_data
                 * \return
                 */
                WellBlock *getWellBlock(Reservoir::WellIndexCalculation::IntersectedCell block_data);
            };

        }
    }
}

#endif // WELLSPLINE_H
