#ifndef RESERVOIR_H
#define RESERVOIR_H

#include "grid/grid.h"
#include "Settings/model.h"

namespace Reservoir {

/*!
 * \brief The Reservoir class represents the entirety of the reservoir. It acts as a
 * wrapper for the grid.
 */
    class Reservoir
    {
    public:
        Reservoir(::Utilities::Settings::Model::Reservoir reservoir_settings);

        Grid::Grid* grid() const { return grid_; } //!< Get the grid associated with the reservoir.

    private:
        Grid::Grid* grid_;
    };

}

#endif // RESERVOIR_H
