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

#include "eclgrid.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace Reservoir {
    namespace Grid {

        ECLGrid::ECLGrid(std::string file_path)
                : Grid(GridSourceType::ECLIPSE, file_path)
        {
            if (!boost::filesystem::exists(file_path))
                throw std::runtime_error("Grid file " + file_path + " not found.");

            std::string init_file_path = file_path;
            if (boost::algorithm::ends_with(file_path, ".EGRID"))
                boost::replace_all(init_file_path, ".EGRID", ".INIT");
            else if (boost::algorithm::ends_with(file_path, ".GRID"))
                boost::replace_all(init_file_path, ".GRID", ".INIT");
            if (!boost::filesystem::exists(init_file_path))
                throw std::runtime_error("ECLGrid::ECLGrid: Reservoir init file " + init_file_path + " not found.");

            ecl_grid_reader_ = new ERTWrapper::ECLGrid::ECLGridReader();
            ecl_grid_reader_->ReadEclGrid(file_path_);
        }

        ECLGrid::~ECLGrid()
        {
            delete ecl_grid_reader_;
        }

        bool ECLGrid::IndexIsInsideGrid(int global_index) {
            return global_index >= 0 && global_index < (Dimensions().nx*Dimensions().ny*Dimensions().nz);
        }

        bool ECLGrid::IndexIsInsideGrid(int i, int j, int k) {
            return i >= 0 && i < Dimensions().nx &&
                   j >= 0 && j < Dimensions().ny &&
                   k >= 0 && k < Dimensions().nz;
        }

        bool ECLGrid::IndexIsInsideGrid(IJKCoordinate *ijk) {
            return IndexIsInsideGrid(ijk->i(), ijk->j(), ijk->k());
        }

        Grid::Dims ECLGrid::Dimensions()
        {
            Dims dims;
            if (type_ == GridSourceType::ECLIPSE) {
                ERTWrapper::ECLGrid::ECLGridReader::Dims eclDims = ecl_grid_reader_->Dimensions();
                dims.nx = eclDims.nx;
                dims.ny = eclDims.ny;
                dims.nz = eclDims.nz;
                return dims;
            }
            else throw std::runtime_error("ECLGrid::Dimensions: Grid source must be defined before getting grid dimensions.");
        }

        Cell ECLGrid::GetCell(int global_index)
        {
            if (!IndexIsInsideGrid(global_index)) throw std::runtime_error("Error getting grid cell. Global index is outside grid.");
            if (type_ == GridSourceType::ECLIPSE) {
                ERTWrapper::ECLGrid::ECLGridReader::Cell ertCell = ecl_grid_reader_->GetGridCell(global_index);

                // Get IJK index
                ERTWrapper::ECLGrid::ECLGridReader::IJKIndex ecl_ijk_index = ecl_grid_reader_->ConvertGlobalIndexToIJK(global_index);
                IJKCoordinate ijk_index = IJKCoordinate(ecl_ijk_index.i, ecl_ijk_index.j, ecl_ijk_index.k);

                // Get center coordinates
                auto center = Eigen::Vector3d(ertCell.center);

                // Get the corners
                std::vector<Eigen::Vector3d> corners;
                for (auto corner : ertCell.corners) {
                    corners.push_back(corner);
                }
                return Cell(global_index, ijk_index,
                            ertCell.volume, ertCell.porosity, ertCell.permx, ertCell.permy, ertCell.permz,
                            center, corners);
            }
            else throw std::runtime_error("ECLGrid::GetCell: Grid source must be defined before getting a cell.");
        }

        Cell ECLGrid::GetCell(int i, int j, int k)
        {
            if (!IndexIsInsideGrid(i, j, k)) throw std::runtime_error("Error getting grid cell. Index (i, j, k) is outside grid.");
            if (type_ == GridSourceType::ECLIPSE) {
                int global_index = ecl_grid_reader_->ConvertIJKToGlobalIndex(i, j, k);
                return GetCell(global_index);
            }
            else throw std::runtime_error("ECLGrid::GetCell: Grid source must be defined before getting a cell.");
        }

        Cell ECLGrid::GetCell(IJKCoordinate *ijk)
        {
            if (!IndexIsInsideGrid(ijk)) throw std::runtime_error("ECLGrid::GetCell: Index ijk is outside grid.");
            if (type_ == GridSourceType::ECLIPSE) {
                int global_index = ecl_grid_reader_->ConvertIJKToGlobalIndex(ijk->i(), ijk->j(), ijk->k());
                return GetCell(global_index);
            }
            else throw std::runtime_error("ECLGrid::GetCell: Grid source must be defined before getting a cell.");
        }

        Cell ECLGrid::GetCellEnvelopingPoint(double x, double y, double z)
        {
            int total_cells = Dimensions().nx * Dimensions().ny * Dimensions().nz;
	    	
            for (int ii = 0; ii < total_cells; ii++) {
                if (GetCell(ii).EnvelopsPoint(Eigen::Vector3d(x, y, z))) {
                    return GetCell(ii);
                }
            }
	    
            // Throw an exception if no cell was found
            throw std::runtime_error("Grid::GetCellEnvelopingPoint: Cell is outside grid ("
                                     + std::to_string(x) + ", "
                                     + std::to_string(y) + ", "
                                     + std::to_string(z) + ")"
            );
        }

        Cell ECLGrid::GetCellEnvelopingPoint(Eigen::Vector3d xyz)
        {
            return GetCellEnvelopingPoint(xyz.x(), xyz.y(), xyz.z());
        }
    }
}
