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

#ifndef ECLGRIDREADER_H
#define ECLGRIDREADER_H

#include <ert/ecl/ecl_grid.h>
#include <ert/ecl/ecl_file.h>
#include <Eigen/Dense>
#include <vector>

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
                 * \brief The Cell struct represents a cell in the grid.
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
                double volume;
                double porosity;
                double permx;
                double permy;
                double permz;
                std::vector<Eigen::Vector3d> corners;
                Eigen::Vector3d center;
            };

            struct IJKIndex {
                int i;
                int j;
                int k;
            };

        private:
            std::string file_name_;
            std::string init_file_name_;
            ecl_grid_type* ecl_grid_;
            ecl_file_type* ecl_file_init_;
            Eigen::Vector3d GetCellCenter(int global_index);
            std::vector<Eigen::Vector3d> GetCellCorners(int global_index);
            double GetCellVolume(int global_index);

            ecl_kw_type *poro_kw_;
            ecl_kw_type *permx_kw_;
            ecl_kw_type *permy_kw_;
            ecl_kw_type *permz_kw_;
	
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
                 * \brief ConvertIJKToGlobalIndex Converts a set of zero-offset (i,j,k) coordinates to the global index to that cell.
                 * \param ijk Zero-offset coordinates for a cell.
                 * \return global index
                 */
            int ConvertIJKToGlobalIndex(IJKIndex ijk);
            int ConvertIJKToGlobalIndex(int i, int j, int k);

            /*!
                 * \brief ConvertGlobalIndexToIJK Converts a global index for a cell to the corresponding zero-offset (i,j,k) coordinates.
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
                 * \brief ActiveCells Number of active cells in the grid that has been read.
                 */
            int ActiveCells();

            /*!
                 * \brief GetGridCell get a Cell struct describing the cell with the specified global index.
                 * \param global_index The global index of the cell to get.
                 * \return Cell struct.
                 */
            Cell GetGridCell(int global_index);

            /*!
                 * \brief GetGlobalIndexOfCellContainingPoint Gets the global index of any cell that envelops/contains the point (x,y,z).
                 *
                 * Searches the grid to check whether any cell envelops the point (x,y,z).
                 * If one is found, the global index is returned; if not, -1 is returned.
                 * An initial guess may be provided: the search will start from/around this global index. If no guess is
                 * provided the search will start at the global index 0.
                 * \param x coordinate in x-direction.
                 * \param y coordinate in y-direction.
                 * \param z coordinate in z-direction.
                 * \param initial_guess (optional) Global index to start search at/around
                 * \return Global index or -1.
                 */
            int GlobalIndexOfCellEnvelopingPoint(double x, double y, double z, int initial_guess=0);

            bool GlobalIndexIsInsideGrid(int global_index);
        };
    }
}

#endif // ECLGRIDREADER_H
