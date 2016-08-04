#ifndef ECLGRID_H
#define ECLGRID_H

#include "grid.h"

namespace Reservoir {
namespace Grid {

/*!
 * \brief The ECLGrid class is an implementation of the abstract Grid class for ECLIPSE grids.
 *
 * This class uses the ERT to read the generated grid files (.GRID or .EGRID) through the ERTWrapper
 * library.
 */
class ECLGrid : public Grid
{
public:
    ECLGrid(QString file_path);
    virtual ~ECLGrid();

    Dims Dimensions();
    Cell GetCell(int global_index);
    Cell GetCell(int i, int j, int k);
    Cell GetCell(IJKCoordinate* ijk);
    Cell GetCellEnvelopingPoint(double x, double y, double z);
    Cell GetCellEnvelopingPoint(Eigen::Vector3d xyz);


private:
    ERTWrapper::ECLGrid::ECLGridReader* ecl_grid_reader_ = 0;
    bool IndexIsInsideGrid(int global_index); //!< Check that global_index is less than nx*ny*nz
    bool IndexIsInsideGrid(int i, int j, int k); //!< Check that (i,j,k) are >= 0 and less than n*.
    bool IndexIsInsideGrid(IJKCoordinate *ijk); //!< Check that (i,j,k) are >= 0 and less than n*.

};

}
}

#endif // ECLGRID_H
