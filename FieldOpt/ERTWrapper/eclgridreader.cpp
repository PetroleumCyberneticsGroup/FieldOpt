#include "eclgridreader.h"
#include <math.h>
#include <iostream>

#include "ertwrapper_exceptions.h"

namespace ERTWrapper {
namespace ECLGrid {
QVector3D ECLGridReader::GetCellCenter(int global_index)
{
    double cx, cy, cz;
    ecl_grid_get_xyz1(ecl_grid_, global_index, &cx, &cy, &cz);
    return QVector3D(cx, cy, cz);
}

QList<QVector3D> ECLGridReader::GetCellCorners(int global_index)
{
    QList<QVector3D> corners = QList<QVector3D>();
    for (int i = 0; i < 8; ++i) {
        double x, y, z;
        ecl_grid_get_cell_corner_xyz1(ecl_grid_, global_index, i, &x, &y, &z);
        corners.append(QVector3D(x, y, z));
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
    ecl_file_init_ = 0;
    poro_kw_ = 0;
    permx_kw_ = 0;
    permy_kw_ = 0;
    permz_kw_ = 0;
}

ECLGridReader::~ECLGridReader()
{
    if (ecl_grid_ != 0)
        ecl_grid_free(ecl_grid_);
    if (ecl_file_init_ != 0)
        ecl_file_close(ecl_file_init_);
}

void ECLGridReader::ReadEclGrid(QString file_name)
{
    file_name_ = file_name;
    if (file_name.endsWith(".EGRID")) init_file_name_ = file_name.replace(".EGRID", ".INIT");
    if (file_name.endsWith(".GRID")) init_file_name_ = file_name.replace(".GRID", ".INIT");

    if (ecl_grid_ == 0) {
        ecl_grid_ = ecl_grid_alloc(file_name_.toStdString().c_str());
    } else {
        ecl_grid_free(ecl_grid_);
        ecl_grid_ = ecl_grid_alloc(file_name_.toStdString().c_str());
    }
    if (ecl_file_init_ == 0) {
        ecl_file_init_ = ecl_file_open(init_file_name_.toStdString().c_str(), 0);
        poro_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PORO", 0);
        permx_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PERMX", 0);
        permy_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PERMY", 0);
        permz_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PERMZ", 0);
    } else {
        ecl_file_close(ecl_file_init_);
        ecl_file_init_ = ecl_file_open(init_file_name_.toStdString().c_str(), 0);
        poro_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PORO", 0);
        permx_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PERMX", 0);
        permy_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PERMY", 0);
        permz_kw_ = ecl_file_iget_named_kw(ecl_file_init_, "PERMZ", 0);
    }
}

int ECLGridReader::ConvertIJKToGlobalIndex(ECLGridReader::IJKIndex ijk)
{
    if (ecl_grid_ == 0) throw GridNotReadException("Grid must be read before converting indices.");
    return ecl_grid_get_global_index3(ecl_grid_, ijk.i, ijk.j, ijk.k);
}

int ECLGridReader::ConvertIJKToGlobalIndex(int i, int j, int k)
{
    if (ecl_grid_ == 0) throw GridNotReadException("Grid must be read before converting indices.");
    return ecl_grid_get_global_index3(ecl_grid_, i, j, k);
}

ECLGridReader::IJKIndex ECLGridReader::ConvertGlobalIndexToIJK(int global_index)
{
    if (ecl_grid_ == 0) throw GridNotReadException("Grid must be read before converting indices.");
    int i, j, k;
    ecl_grid_get_ijk1(ecl_grid_, global_index, &i, &j, &k);
    ECLGridReader::IJKIndex ijk;
    ijk.i = i; ijk.j = j; ijk.k = k;
    return ijk;
}

ECLGridReader::Dims ECLGridReader::Dimensions()
{
    ECLGridReader::Dims dims;
    if (ecl_grid_ == 0) throw GridNotReadException("Grid must be read before getting dimensions.");
    int x, y, z;
    ecl_grid_get_dims(ecl_grid_, &x, &y, &z, NULL);
    dims.nx = x; dims.ny = y; dims.nz = z;
    return dims;
}

int ECLGridReader::ActiveCells()
{
    if (ecl_grid_ == 0) throw GridNotReadException("Grid must be read before gettign active cells.");
    else return ecl_grid_get_nactive(ecl_grid_);
}

ECLGridReader::Cell ECLGridReader::GetGridCell(int global_index)
{
    if (!GlobalIndexIsInsideGrid(global_index))
        throw InvalidIndexException("The global index " + std::to_string(global_index) + " is outside the grid.");
    if (ecl_grid_ == 0) throw GridNotReadException("Grid must be read before getting grid cells.");
    ECLGridReader::Cell cell;
    cell.global_index = global_index;
    cell.volume = GetCellVolume(global_index);
    cell.corners = GetCellCorners(global_index);
    cell.center = GetCellCenter(global_index);

    // Get properties from the INIT file
    cell.porosity = ecl_kw_iget_as_double(poro_kw_, global_index);
    cell.permx = ecl_kw_iget_as_double(permx_kw_, global_index);
    cell.permy = ecl_kw_iget_as_double(permy_kw_, global_index);
    cell.permz = ecl_kw_iget_as_double(permz_kw_, global_index);

    return cell;
}

int ECLGridReader::GlobalIndexOfCellEnvelopingPoint(double x, double y, double z, int initial_guess)
{
    if (ecl_grid_ == 0) throw GridNotReadException("Grid must be read before searching for cells.");
    return ecl_grid_get_global_index_from_xyz(ecl_grid_, x, y, z, initial_guess);
}

bool ECLGridReader::GlobalIndexIsInsideGrid(int global_index)
{
    Dims dims = Dimensions();
    if (global_index < dims.nx * dims.ny * dims.nz)
        return true;
    else
        return false;
}



}
}
