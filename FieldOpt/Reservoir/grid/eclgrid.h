/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef ECLGRID_H
#define ECLGRID_H

#include "grid.h"

namespace Reservoir {
namespace Grid {

/*!
 * \brief The ECLGrid class is an implementation
 * of the abstract Grid class for ECLIPSE grids.
 *
 * This class uses the ERT to read the generated grid
 * files (.GRID or .EGRID) through the ERTWrapper library.
 */
class ECLGrid : public Grid
{
 public:
  ECLGrid(std::string file_path);
  virtual ~ECLGrid();

  Dims Dimensions() override;
  Cell GetCell(int global_index) override;
  Cell GetCell(int i, int j, int k) override;
  Cell GetCell(IJKCoordinate* ijk) override;
  void FillCellProperties(Cell &cell) override;
  Cell GetCellEnvelopingPoint(double x, double y, double z) override;
  Cell GetCellEnvelopingPoint(Eigen::Vector3d xyz) override;


 private:
  ERTWrapper::ECLGrid::ECLGridReader* ecl_grid_reader_ = 0;
  bool IndexIsInsideGrid(int global_index); //!< Check that global_index is less than nx*ny*nz
  bool IndexIsInsideGrid(int i, int j, int k); //!< Check that (i,j,k) are >= 0 and less than n*.
  bool IndexIsInsideGrid(IJKCoordinate *ijk); //!< Check that (i,j,k) are >= 0 and less than n*.

};

}
}

#endif // ECLGRID_H
