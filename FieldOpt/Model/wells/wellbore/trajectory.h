#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "wellblock.h"
#include "wellspline.h"
#include "completions/completion.h"
#include "completions/perforation.h"
#include "Reservoir/reservoir.h"
#include "Settings/model.h"
#include "Model/properties/variable_property_container.h"
#include "Model/properties/property.h"

#include <QList>

namespace Model {
    namespace Wells {
        namespace Wellbore {

            class WellSpline;

            /*!
             * \brief The Trajectory class describes the trajectory of the wellbore as a set of well blocks
             * in the reservoir grid.
             *
             * A trajectory may be described either as a set of well blocks directly, or as a spline which
             * a set of well blocks is calculated from. Either way it is always percieved as a set of blocks
             * from the outside.
             *
             * \todo Initialize ICDs.
             */
            class Trajectory
            {
            public:
                Trajectory(::Utilities::Settings::Model::Well well_settings,
                           Properties::VariablePropertyContainer *variable_container,
                           Reservoir::Reservoir *reservoir);
                WellBlock *GetWellBlock(int i, int j, int k); //!< Get the well block at index (i,j,k).
                QList<WellBlock *> *GetWellBlocks(); //!< Get a list containing all well blocks.
                void UpdateWellBlocks(); //!< Update the well blocks, in particular the ones defined by a spline.

            private:
                QList<WellBlock *> *well_blocks_;
                WellSpline *well_spline_; //!< Used to defined trajectories with a spline. When used, this generates the well blocks.

                void initializeWellBlocks(Utilities::Settings::Model::Well well,
                                          Properties::VariablePropertyContainer *variable_container);

                void calculateDirectionOfPenetration(); // Calculate direction of penetration for all well blocks

            };

        }
    }
}

#endif // TRAJECTORY_H
