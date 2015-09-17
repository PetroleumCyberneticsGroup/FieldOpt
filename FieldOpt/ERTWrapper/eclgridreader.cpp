#include "eclgridreader.h"
#include <math.h>
#include <iostream>

namespace ERTWrapper {
namespace ECLGrid {
QVector3D* ECLGridReader::GetCellCenter(int global_index)
{
    double cx, cy, cz;
    ecl_grid_get_xyz1(ecl_grid_, global_index, &cx, &cy, &cz);
    return new QVector3D(cx, cy, cz);
}

QList<QVector3D *> *ECLGridReader::GetCellCorners(int global_index)
{
    QList<QVector3D*>* corners = new QList<QVector3D*>();
    for (int i = 0; i < 8; ++i) {
        double x, y, z;
        ecl_grid_get_cell_corner_xyz1(ecl_grid_, global_index, i, &x, &y, &z);
        corners->append(new QVector3D(x, y, z));
    }
    return corners;
}

double ECLGridReader::GetCellVolume(int global_index)
{
    return ecl_grid_get_cell_volume1(ecl_grid_, global_index);
}

ECLGridReader::ECLGridReader()
{
    ecl_grid_ = 0;
}

ECLGridReader::~ECLGridReader()
{
    if (ecl_grid_ != 0)
        ecl_grid_free(ecl_grid_);
}

void ECLGridReader::ReadEclGrid(QString file_name)
{
    file_name_ = file_name;
    if (ecl_grid_ == 0) {
        ecl_grid_ = ecl_grid_alloc(file_name.toStdString().c_str());
    }
    else {
        ecl_grid_free(ecl_grid_);
        ecl_grid_ = ecl_grid_alloc(file_name.toStdString().c_str());
    }
}

int ECLGridReader::ConvertIJKToGlobalIndex(ECLGridReader::IJKIndex ijk)
{
    if (ecl_grid_ == 0) return 0; // Return 0 if the grid has not been read
    return ecl_grid_get_global_index3(ecl_grid_, ijk.i, ijk.j, ijk.k);
}

int ECLGridReader::ConvertIJKToGlobalIndex(int i, int j, int k)
{
    if (ecl_grid_ == 0) return 0; // Return 0 if the grid has not been read
    return ecl_grid_get_global_index3(ecl_grid_, i, j, k);
}

ECLGridReader::IJKIndex ECLGridReader::ConvertGlobalIndexToIJK(int global_index)
{
    int i, j, k;
    ecl_grid_get_ijk1(ecl_grid_, global_index, &i, &j, &k);
    ECLGridReader::IJKIndex ijk;
    ijk.i = i; ijk.j = j; ijk.k = k;
    return ijk;
}

ECLGridReader::Dims ECLGridReader::Dimensions()
{
    ECLGridReader::Dims dims;
    if (ecl_grid_ == 0) {
        dims.nx = 0; dims.ny = 0; dims.nz = 0;
        return dims;
    }
    int x, y, z;
    ecl_grid_get_dims(ecl_grid_, &x, &y, &z, NULL);
    dims.nx = x; dims.ny = y; dims.nz = z;
    return dims;
}

int ECLGridReader::ActiveCells()
{
    if (ecl_grid_ == 0) return 0; // Return 0 if the grid has not been read
    else return ecl_grid_get_nactive(ecl_grid_);
}

ECLGridReader::Cell ECLGridReader::GetGridCell(int global_index)
{
    ECLGridReader::Cell cell;
    cell.global_index = global_index;
    cell.volume = GetCellVolume(global_index);
    cell.corners = GetCellCorners(global_index);
    cell.center = GetCellCenter(global_index);
    return cell;
}

int ECLGridReader::GlobalIndexOfCellEnvelopingPoint(double x, double y, double z, int initial_guess)
{
    return ecl_grid_get_global_index_from_xyz(ecl_grid_, x, y, z, initial_guess);
}



}
}
