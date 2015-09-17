#include "grid.h"
#include <QVector3D>
#include <QList>

namespace Model {
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
    else {
        dims.nx = 0;
        dims.ny = 0;
        dims.nz = 0;
        return dims;
    }
}

Cell Grid::GetCell(int global_index)
{
    if (type_ == GridSourceType::ECLIPSE) {
        ERTWrapper::ECLGrid::ECLGridReader::Cell ertCell = ecl_grid_reader_->GetGridCell(global_index);

        // Get IJK index
        QVector3D ijk_index_real = ecl_grid_reader_->ConvertGlobalIndexToIJK(global_index);
        IJKCoordinate* ijk_index = new IJKCoordinate(ijk_index_real.x(), ijk_index_real.y(), ijk_index_real.z());

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
    else {
        return Cell();
    }
}



}
}
