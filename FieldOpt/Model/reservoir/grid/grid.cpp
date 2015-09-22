#include "grid.h"
#include <QVector3D>
#include <QList>
#include <iostream>
#include <string>

#include "grid_exceptions.h"

namespace Model {
namespace Reservoir {
namespace Grid {

Grid::Grid(Grid::GridSourceType type, QString file_path)
{
    type_ = type;
    file_path_ = file_path;

    if (type_ == GridSourceType::ECLIPSE) {
        ecl_grid_reader_ = new ERTWrapper::ECLGrid::ECLGridReader();
        ecl_grid_reader_->ReadEclGrid(file_path_);
    }
}

Grid::~Grid()
{
    if (type_ == GridSourceType::ECLIPSE)
        delete ecl_grid_reader_;
}

bool Grid::IndexIsInsideGrid(int global_index) {
    return global_index >= 0 && global_index < (Dimensions().nx*Dimensions().ny*Dimensions().nz);
}

bool Grid::IndexIsInsideGrid(int i, int j, int k) {
    return i >= 0 && i < Dimensions().nx &&
            j >= 0 && j < Dimensions().ny &&
            k >= 0 && k < Dimensions().nz;
}

bool Grid::IndexIsInsideGrid(IJKCoordinate *ijk) {
    return IndexIsInsideGrid(ijk->i(), ijk->j(), ijk->k());
}

Grid::Dims Grid::Dimensions()
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

Cell Grid::GetCell(int global_index)
{
    if (!IndexIsInsideGrid(global_index)) throw CellIndexOutsideGridException("Global index is outside grid.");
    if (type_ == GridSourceType::ECLIPSE) {
        ERTWrapper::ECLGrid::ECLGridReader::Cell ertCell = ecl_grid_reader_->GetGridCell(global_index);

        // Get IJK index
        ERTWrapper::ECLGrid::ECLGridReader::IJKIndex ecl_ijk_index = ecl_grid_reader_->ConvertGlobalIndexToIJK(global_index);
        IJKCoordinate* ijk_index = new IJKCoordinate(ecl_ijk_index.i, ecl_ijk_index.j, ecl_ijk_index.k);

        // Get center coordinates
        XYZCoordinate* center = new XYZCoordinate(ertCell.center->x(), ertCell.center->y(), ertCell.center->z());

        // Get the corners
        QList<XYZCoordinate*> corners = QList<XYZCoordinate*>();
        for (int i = 0; i < ertCell.corners->size(); ++i) {
            corners.append(new XYZCoordinate(
                               ertCell.corners->at(i)->x(),
                               ertCell.corners->at(i)->y(),
                               ertCell.corners->at(i)->z()));
        }
        return Cell(global_index, ijk_index, ertCell.volume, center, corners);
    }
    else throw GridCellNotFoundException("Grid source must be defined before getting a cell.");
}

Cell Grid::GetCell(int i, int j, int k)
{
    if (!IndexIsInsideGrid(i, j, k)) throw CellIndexOutsideGridException("Index (i, j, k) is outside grid.");
    if (type_ == GridSourceType::ECLIPSE) {
        int global_index = ecl_grid_reader_->ConvertIJKToGlobalIndex(i, j, k);
        return GetCell(global_index);
    }
    else throw GridCellNotFoundException("Grid source must be defined before getting a cell.");
}

Cell Grid::GetCell(IJKCoordinate *ijk)
{
    if (!IndexIsInsideGrid(ijk)) throw CellIndexOutsideGridException("Index ijk is outside grid.");
    if (type_ == GridSourceType::ECLIPSE) {
        int global_index = ecl_grid_reader_->ConvertIJKToGlobalIndex(ijk->i(), ijk->j(), ijk->k());
        return GetCell(global_index);
    }
    else throw GridCellNotFoundException("Grid source must be defined before getting a cell.");
}

Cell Grid::GetCellEnvelopingPoint(double x, double y, double z)
{
    int global_index = ecl_grid_reader_->GlobalIndexOfCellEnvelopingPoint(x, y ,z);
    if (global_index == -1) {
        throw GridCellNotFoundException("No grid cell found enveloping point.");
    }
    return GetCell(global_index);
}

Cell Grid::GetCellEnvelopingPoint(XYZCoordinate *xyz)
{
    return GetCellEnvelopingPoint(xyz->x(), xyz->y(), xyz->z());
}





}
}
}
