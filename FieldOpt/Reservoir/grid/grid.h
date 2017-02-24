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


#ifndef GRID_H
#define GRID_H

#include "cell.h"
#include "ijkcoordinate.h"
#include "ERTWrapper/eclgridreader.h"

namespace Reservoir {
namespace Grid {

/*!
 * \brief The abstract Grid class represents a reservoir grid. It indcludes basic operations for lookup,
 * checks and calculations on grids.
 */
class Grid
{
 public:
  /*!
   * \brief The GridSourceType enum Enumerates the supported grid types.
   */
  enum GridSourceType {ECLIPSE};
  Grid(const Grid& other) = delete;

  /*!
   * \brief The Dims struct Contains the grid dimensions.
   */
  struct Dims {
    int nx; int ny; int nz;
  };

  virtual ~Grid();

  /*!
   * \brief Dimensions Returns the grid dimensions (number of blocks in x, y and z directions).
   * \return
   */
  virtual Dims Dimensions() = 0;

  /*!
   * \brief GetCell Get a cell from its global index.
   * \note This only gets the cell geometry.
   */
  virtual Cell GetCell(int global_index) = 0;

  /*!
   * \brief GetCell Get a cell from its (i,j,k) index.
   * \note This only gets the cell geometry.
   */
  virtual Cell GetCell(int i, int j, int k) = 0;

  /*!
   * \brief GetCell Get a cell from its (i,j,k) index.
   * \note This only gets the cell geometry.
   */
  virtual Cell GetCell(IJKCoordinate* ijk) = 0;

  /*!
   * @brief Fill in the non-geometry properties of a cell/
   * @param cell The cell to get the properties for.
   */
  virtual void FillCellProperties(Cell &cell) = 0;

  /*!
   * \brief GetCellEnvelopingPoint Get the cell enveloping the point (x,y,z). Throws an exception if no cell is found.
   */
  virtual Cell GetCellEnvelopingPoint(double x, double y, double z) = 0;

  /*!
   * \brief GetCellEnvelopingPoint Get the cell enveloping the point (x,y,z). Throws an exception if no cell is found.
   */
  virtual Cell GetCellEnvelopingPoint(Eigen::Vector3d xyz) = 0;

 protected:
  GridSourceType type_;
  std::string file_path_;
  Grid(GridSourceType type, std::string file_path);
};

}
}


#endif // GRID_H
