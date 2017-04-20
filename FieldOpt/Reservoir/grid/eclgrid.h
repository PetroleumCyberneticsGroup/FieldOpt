/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>

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

#include <vector>
#include "grid.h"

namespace Reservoir {
namespace Grid {

using namespace std;

/*!
 * \brief The ECLGrid class is an implementation
 * of the abstract Grid class for ECLIPSE grids.
 *
 * This class uses the ERT to read the generated grid
 * files (.GRID or .EGRID) through the ERTWrapper library.
 */
class ECLGrid : public Grid
{
private:
	int faces_permutation_index; 
	
 public:
  ECLGrid(std::string file_path);
  virtual ~ECLGrid();

  Dims Dimensions();
  Cell GetCell(int global_index);
  Cell GetCell(int i, int j, int k);
  Cell GetCell(IJKCoordinate* ijk);

  vector<int> GetBoundingBoxCellIndices(
      double xi, double yi, double zi,
      double xf, double yf, double zf,
      double &bb_xi, double &bb_yi, double &bb_zi,
      double &bb_xf, double &bb_yf, double &bb_zf);

  Cell GetCellEnvelopingPoint(double x, double y, double z);
  Cell GetCellEnvelopingPoint(double x, double y, double z,
                              vector<int> search_set);

  Cell GetCellEnvelopingPoint(Eigen::Vector3d xyz);
  Cell GetCellEnvelopingPoint(Eigen::Vector3d xyz,
                              vector<int> search_set);

 private:
  ERTWrapper::ECLGrid::ECLGridReader* ecl_grid_reader_ = 0;

  //!< Check that global_index is less than nx*ny*nz
  bool IndexIsInsideGrid(int global_index);

  //!< Check that (i,j,k) are >= 0 and less than n*.
  bool IndexIsInsideGrid(int i, int j, int k);

  //!< Check that (i,j,k) are >= 0 and less than n*.
  bool IndexIsInsideGrid(IJKCoordinate *ijk);

  bool SetGridCellFacesPermutations();
};

}
}

#endif // ECLGRID_H
