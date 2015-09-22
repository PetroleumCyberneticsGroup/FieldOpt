#ifndef GRID_H
#define GRID_H

#include <QString>
#include "cell.h"
#include "ijkcoordinate.h"
#include "xyzcoordinate.h"
#include "eclgridreader.h"

namespace Model {
namespace Reservoir {
namespace Grid {

/*!
 * \brief The Grid class represents a reservoir grid. It indcludes basic operations for lookup,
 * checks and calculations on grids. Currently only ECLIPSE grids are supported.
 */
class Grid
{
public:
    /*!
     * \brief The GridSourceType enum Enumerates the supported grid types.
     */
    enum GridSourceType {ECLIPSE};

    /*!
     * \brief The Dims struct Contains the grid dimensions.
     */
    struct Dims {
        int nx; int ny; int nz;
    };

private:
    GridSourceType type_;
    QString file_path_;
    ERTWrapper::ECLGrid::ECLGridReader* ecl_grid_reader_ = 0;
    bool IndexIsInsideGrid(int global_index); //!< Check that global_index is less than nx*ny*nz
    bool IndexIsInsideGrid(int i, int j, int k); //!< Check that (i,j,k) are >= 0 and less than n*.
    bool IndexIsInsideGrid(IJKCoordinate *ijk); //!< Check that (i,j,k) are >= 0 and less than n*.

public:
    Grid(GridSourceType type, QString file_path);
    virtual ~Grid();

    /*!
     * \brief Dimensions Returns the grid dimensions (number of blocks in x, y and z directions).
     * \return
     */
    Dims Dimensions();

    /*!
     * \brief GetCell Get a cell from its global index.
     */
    Cell GetCell(int global_index);

    /*!
     * \brief GetCell Get a cell from its (i,j,k) index.
     */
    Cell GetCell(int i, int j, int k);

    /*!
     * \brief GetCell Get a cell from its (i,j,k) index.
     */
    Cell GetCell(IJKCoordinate* ijk);

    /*!
     * \brief GetCellEnvelopingPoint Get the cell enveloping the point (x,y,z). Throws an exception if no cell is found.
     */
    Cell GetCellEnvelopingPoint(double x, double y, double z);

    /*!
     * \brief GetCellEnvelopingPoint Get the cell enveloping the point (x,y,z). Throws an exception if no cell is found.
     */
    Cell GetCellEnvelopingPoint(XYZCoordinate* xyz);
};

}
}
}

#endif // GRID_H
