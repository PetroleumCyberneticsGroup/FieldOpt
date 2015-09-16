#include "eclgridreader.h"
#include <math.h>
#include <iostream>

namespace ERTWrapper {
    namespace ECLGrid {
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

        int ECLGridReader::ConvertIJKToGlobalIndex(QVector3D ijk)
        {
            if (ecl_grid_ == 0) return 0; // Return 0 if the grid has not been read
            return ecl_grid_get_global_index3(ecl_grid_, ijk.x(), ijk.y(), ijk.z());
        }

        QVector3D ECLGridReader::ConvertGlobalIndexToIJK(int global_index)
        {
            int i, j, k;
            ecl_grid_get_ijk1(ecl_grid_, global_index, &i, &j, &k);
            return QVector3D(i, j, k);
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

            // Get corners
            cell.corners = new QList<QVector3D*>();
            for (int i = 0; i < 8; ++i) {
                double x, y, z;
                ecl_grid_get_cell_corner_xyz1(ecl_grid_, global_index, i, &x, &y, &z);
                cell.corners->append(new QVector3D(x, y, z));
            }

            // Get center
            double cx, cy, cz;
            ecl_grid_get_xyz1(ecl_grid_, global_index, &cx, &cy, &cz);
            cell.center = new QVector3D(cx, cy, cz);

            return cell;
        }



    }
}
