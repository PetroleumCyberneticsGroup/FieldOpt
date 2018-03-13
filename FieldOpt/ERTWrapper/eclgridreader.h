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

#ifndef ECLGRIDREADER_H
#define ECLGRIDREADER_H

// -----------------------------------------------------------------
// ERT
#include <ert/ecl/ecl_grid.h>
#include <ert/ecl/ecl_file.h>
#include <ert/ecl/ecl_kw.h>
#include <ert/ecl/ecl_kw_magic.h>

// -----------------------------------------------------------------
// EIGEN
#include <Eigen/Dense>

// -----------------------------------------------------------------
// STD
#include <vector>

using namespace std;
using namespace Eigen;

namespace ERTWrapper {
namespace ECLGrid {

/*!
 * \brief The ECLGridReader class reads ECLIPSE grid files (.GRID and .EGRID).
 *
 * \note ERT uses 0-index IJK indeces. These are not directly valid in ECL or
 * ADGPRS simulator -- they need to be incremented by 1.
 */
class ECLGridReader
{
 public:
  /*!
   * \brief The Dims struct holds the x,y and z dimensions of a grid.
   */
  struct Dims {
    int nx, ny, nz;
  };

  /*!
   * \brief The Index struct holds a series of indexing vectors that
   * correspond to the total number of cells, as well as indexing of
   * active/inactive cells in the grid.
   *
   * See hdf5_summary_reader.h for similiar type of variables
   */
  struct Gidx {
    int n_total; //!< total number of cells = nx*ny*nz
    int n_active; //!< total number of active cells

    /*! vector (sz=n_total) containing sorted indices of all cells
     * in grid, i.e., list of global index
     * */
    VectorXi idx_total;

    /*! vector (sz=n_total) containing active cells indices based on
     * actnum data, i.e., only those vector components that correspond
     * to active cells are nonzero (set equal to their associated global
     * indices), all other components corresponding to inactive cells
     * are equal to zero
     */
    VectorXd idx_actnum;

    /*! vector (sz=n_total) containing actual actnum data */
    VectorXd dat_actnum;

    /*! vector (sz=n_active) containing global indices of each active cell */
    VectorXi idx_active;
  };

  /*!
   * \brief The Cell struct represents a cell in the grid.
   *
   * The cell struct also contains all non-geometric properties
   * for a cell, i.e. permeabilities, porosities, and whether or
   * not the cell is active.
   *
   * In case of a dual grid the active status of matrix and fracture
   * will be recorded and the properties for all grids is stored.
   *
   * The ecl_grid specifies the active status is encoded as:
   *                  0 - inactive,
   *                  1 - active in matrix,
   *                  2 - active in fracture
   *                  3 - active in both.
   * Here we use two booleans to store the active status.
   * The properties are stored in a vector with one or two items depending
   * on the active status. If the cell is active both in the matrix grid as
   * well as in the fracture grid the matrix values will be stored first.
   *
   * The corners list contains all the corner points specifying the grid.
   * The first four elements represent the corners in the lower layer,
   * the four last represent the corners in the top layer:
   *
   * Bottom:  Top:
   * 2---3    6---7
   * |   |    |   |
   * 0---1    4---5
   *
   * Bottom and Top here refers to z-values.
   */
  struct Cell {
    int global_index;
    bool matrix_active;
    bool fracture_active;
    double volume;
    std::vector<double> porosity;
    std::vector<double> permx;
    std::vector<double> permy;
    std::vector<double> permz;
    std::vector<Eigen::Vector3d> corners;
    Eigen::Vector3d center;
    Eigen::Vector3d dxdydz;
  };

  struct IJKIndex {
    int i;
    int j;
    int k;
  };

  struct GridData {
    VectorXd coord, coord_rxryrz;
    Matrix<double, Dynamic, 3, RowMajor> coord_xyz, coord_rxryrz_xyz;
    VectorXd zcorn, zcorn_rxryrz;
  };

  Gidx gidx_;
  GridData gridData_;

 private:
string file_name_;
  string init_file_name_;
  string egrid_file_name_;

  ecl_grid_type* ecl_grid_;
  ecl_file_type* ecl_file_grid_;
  ecl_file_type* ecl_file_init_;
  ecl_grid_type* ecl_file_egrid_;
  Eigen::Vector3d GetCellCenter(int global_index);
  Eigen::Vector3d GetCellDxDyDz(int global_index);
  std::vector<Eigen::Vector3d> GetCellCorners(int global_index);
  double GetCellVolume(int global_index);

  ecl_kw_type *poro_kw_;
  ecl_kw_type *permx_kw_;
  ecl_kw_type *permy_kw_;
  ecl_kw_type *permz_kw_;
  ecl_kw_type *actnum_kw_;
  ecl_kw_type *coord_kw_;
  ecl_kw_type *zcorn_kw_;

 public:
  ECLGridReader();
  ECLGridReader(const ECLGridReader& other) = delete;
  virtual ~ECLGridReader(); //!< Frees the grid object if it has been set.

  /*!
   * \brief ReadEclGrid reads an ECLIPSE .GRID or .EGRID file.
   * \param file_name The path to the grid to be read, including suffix.
   */
  void ReadEclGrid(std::string file_name);

  /*!
   * \brief ConvertIJKToGlobalIndex Converts a set of zero-offset
   * (i,j,k) coordinates to the global index to that cell.
   * \param ijk Zero-offset coordinates for a cell.
   * \return global index
   */
  int ConvertIJKToGlobalIndex(IJKIndex ijk);
  int ConvertIJKToGlobalIndex(int i, int j, int k);

  /*!
   * \brief ConvertMatrixActiveIndexToGlobalIndex Converts a zero-offset index in the set of cells active in the matrix grid
   * to the global index.
   * \param index Zero-offset index for a cell active in the matrix grid
   * \return global index
   */
  int ConvertMatrixActiveIndexToGlobalIndex(int index);

  /*!
   * \brief ConvertGlobalIndexToIJK Converts a global index for a
   * cell to the corresponding zero-offset (i,j,k) coordinates.
   * \param global_index Global index for a cell.
   * \return (i,j,k) Zero-offset coordinates
   */
  IJKIndex ConvertGlobalIndexToIJK(int global_index);

  /*!
   * \brief Dimensions returns the total dimensions of the grid that has been read.
   * \return Dims struct containing the number of blocks in x, y and z direction.
   */
  Dims Dimensions();

  /*!
   * \brief
   * \param
   * \return
   */
  void GetCOORDZCORNData();

  /*!
   * \brief
   * \param
   * \return
   */
  void GetGridSummary();

  /*!
   * \brief Retrieve useful indices that specify total number of grid cells,
   * and which cells are active/inactive
   * \return Gidx struc
   */
  void GetGridIndices();

  /*!
   * \brief ActiveCells Number of active cells in the grid that has been read.
   */
  int ActiveCells();

  /*!
   * \brief NumActiveCells Number of active cells in the matrix in the grid that has been read.
   */
  int NumActiveMatrixCells();

  /*!
   * \brief NumActiveFractureCells Number of active cells in the fracture grid that has been read.
   * This only makes sense for dual grids
   */
  int NumActiveFractureCells();

  /*!
   * \brief IsCellActive returns false if the cell identified by its global index
   * is not active in the matrix grid nor in the fracture
   */
  bool IsCellActive(int global_index);

  /*!
   * \brief IsCellMatrixActive returns false if the cell identified by its global index
   * is not active in the matrix grid
   */
  bool IsCellMatrixActive(int global_index);

  /*!
   * \brief IsCellFractureActive returns false if the cell identified by its global index
   * is not active in the facture grid in the case of dual grid
   */
  bool IsCellFractureActive(int global_index);

  /*!
   * \brief GetGridCell get a Cell struct describing the cell with the specified global index.
   * \param global_index The global index of the cell to get.
   * \return Cell struct.
   */
  Cell GetGridCell(int global_index);

  /*!
   * \brief GetCellCorners returns xyz corners (8) of cell
   * \param global_index Global index for a cell.
   * \return xyz corners
   */
//  vector<Vector3d> GetCellCorners(int global_index);

  /*!
   * \brief GetCellCorners returns xyz corners (8) of cell
   * \param global_index Global index for a cell.
   * \return xyz corners
   */
  MatrixXd GetCellCornersM(int global_index);

  /*!

  /*!
   * \brief GetGlobalIndexOfCellContainingPoint Gets the global index of any cell
   * that envelops/contains the point (x,y,z).
   *
   * Searches the grid to check whether any cell envelops the point (x,y,z).
   * If one is found, the global index is returned; if not, -1 is returned.
   * An initial guess may be provided: the search will start from/around this
   * global index. If no guess is provided the search will start at the global
   * index 0.
   * \param x coordinate in x-direction.
   * \param y coordinate in y-direction.
   * \param z coordinate in z-direction.
   * \param initial_guess (optional) Global index to start search at/around
   * \return Global index or -1.
   */
  int GlobalIndexOfCellEnvelopingPoint(double x,
                                       double y,
                                       double z,
                                       int initial_guess=0);

  bool GlobalIndexIsInsideGrid(int global_index);

  /*!
   * @brief Find the cell in the reservoir with the smallest volume.
   * @return The smallest cell in the reservoir.
   */
  Cell FindSmallestCell();
};
}
}

#endif // ECLGRIDREADER_H
