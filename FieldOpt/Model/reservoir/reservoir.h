/******************************************************************************
 *
 * reservoir.h
 *
 * Created: 22.09.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#ifndef RESERVOIR_H
#define RESERVOIR_H

#include "grid/grid.h"
#include "Utilities/settings/model.h"

namespace Model {
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
}

#endif // RESERVOIR_H
