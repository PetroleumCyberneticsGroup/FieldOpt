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

#include "eclgrid.h"
#include "grid_exceptions.h"
#include "Utilities/file_handling/filehandling.h"

namespace Model {
namespace Reservoir {
namespace Grid {

ECLGrid::ECLGrid(QString file_path)
    : Grid(GridSourceType::ECLIPSE, file_path)
{
    if (!Utilities::FileHandling::FileExists(file_path))
        throw std::runtime_error("Grid file " + file_path.toStdString() + " not found");
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
    else throw GridCellNotFoundException("Grid source must be defined before getting grid dimensions.");
}

Cell ECLGrid::GetCell(int global_index)
{
    if (!IndexIsInsideGrid(global_index)) throw CellIndexOutsideGridException("Global index is outside grid.");
    if (type_ == GridSourceType::ECLIPSE) {
        ERTWrapper::ECLGrid::ECLGridReader::Cell ertCell = ecl_grid_reader_->GetGridCell(global_index);

        // Get IJK index
        ERTWrapper::ECLGrid::ECLGridReader::IJKIndex ecl_ijk_index = ecl_grid_reader_->ConvertGlobalIndexToIJK(global_index);
        IJKCoordinate ijk_index = IJKCoordinate(ecl_ijk_index.i, ecl_ijk_index.j, ecl_ijk_index.k);

        // Get center coordinates
        auto center = XYZCoordinate(ertCell.center.x(), ertCell.center.y(), ertCell.center.z());

        // Get the corners
        QList<XYZCoordinate> corners = QList<XYZCoordinate>();
        for (int i = 0; i < ertCell.corners.size(); ++i) {
            corners.append(XYZCoordinate(
                            ertCell.corners.at(i).x(),
                            ertCell.corners.at(i).y(),
                            ertCell.corners.at(i).z()));
        }
        return Cell(global_index, ijk_index,
                        ertCell.volume, ertCell.porosity, ertCell.permx, ertCell.permy, ertCell.permz,
                        center, corners);
    }
    else throw GridCellNotFoundException("Grid source must be defined before getting a cell.");
}

Cell ECLGrid::GetCell(int i, int j, int k)
{
    if (!IndexIsInsideGrid(i, j, k)) throw CellIndexOutsideGridException("Index (i, j, k) is outside grid.");
    if (type_ == GridSourceType::ECLIPSE) {
        int global_index = ecl_grid_reader_->ConvertIJKToGlobalIndex(i, j, k);
        return GetCell(global_index);
    }
    else throw GridCellNotFoundException("Grid source must be defined before getting a cell.");
}

Cell ECLGrid::GetCell(IJKCoordinate *ijk)
{
    if (!IndexIsInsideGrid(ijk)) throw CellIndexOutsideGridException("Index ijk is outside grid.");
    if (type_ == GridSourceType::ECLIPSE) {
        int global_index = ecl_grid_reader_->ConvertIJKToGlobalIndex(ijk->i(), ijk->j(), ijk->k());
        return GetCell(global_index);
    }
    else throw GridCellNotFoundException("Grid source must be defined before getting a cell.");
}

Cell ECLGrid::GetCellEnvelopingPoint(double x, double y, double z)
{
    int global_index = ecl_grid_reader_->GlobalIndexOfCellEnvelopingPoint(x, y ,z);
    if (global_index == -1) {
        throw GridCellNotFoundException("No grid cell found enveloping point.");
    }
    return GetCell(global_index);
}

Cell ECLGrid::GetCellEnvelopingPoint(XYZCoordinate *xyz)
{
    return GetCellEnvelopingPoint(xyz->x(), xyz->y(), xyz->z());
}

}
}
}
