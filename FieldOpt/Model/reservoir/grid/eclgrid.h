/******************************************************************************
 *
 *
 *
 * Created: 15.10.2015 2015 by einar
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

#ifndef ECLGRID_H
#define ECLGRID_H

#include "grid.h"

namespace Model {
namespace Reservoir {
namespace Grid {

/*!
 * \brief The ECLGrid class is an implementation of the abstract Grid class for ECLIPSE grids.
 *
 * This class uses the ERT to read the generated grid files (.GRID or .EGRID) throug the ERTWrapper
 * library.
 */
class ECLGrid : public Grid
{
public:
    ECLGrid(QString file_path);
    virtual ~ECLGrid();

    Dims Dimensions();
    Cell GetCell(int global_index);
    Cell GetCell(int i, int j, int k);
    Cell GetCell(IJKCoordinate* ijk);
    Cell GetCellEnvelopingPoint(double x, double y, double z);
    Cell GetCellEnvelopingPoint(XYZCoordinate* xyz);


private:
    ERTWrapper::ECLGrid::ECLGridReader* ecl_grid_reader_ = 0;
    bool IndexIsInsideGrid(int global_index); //!< Check that global_index is less than nx*ny*nz
    bool IndexIsInsideGrid(int i, int j, int k); //!< Check that (i,j,k) are >= 0 and less than n*.
    bool IndexIsInsideGrid(IJKCoordinate *ijk); //!< Check that (i,j,k) are >= 0 and less than n*.

};

}
}
}

#endif // ECLGRID_H
