#ifndef GRID_H
#define GRID_H

#include <QString>
#include "cell.h"
#include "ijkcoordinate.h"
#include "xyzcoordinate.h"
#include "ERTWrapper/eclgridreader.h"

namespace Model {
namespace Reservoir {
namespace Grid {

/*!
 * \brief The abstract Grid class represents a reservoir grid. It indcludes basic operations for lookup,
 * checks and calculations on grids.
 */
class Grid
{
public:
    /*!
     * \brief The GridSourceType enum Enumerates the supported grid types.
     */
    enum GridSourceType {ECLIPSE};
    Grid(const Grid& other) = delete;

    /*!
     * \brief The Dims struct Contains the grid dimensions.
     */
    struct Dims {
        int nx; int ny; int nz;
    };

    virtual ~Grid();

    /*!
     * \brief Dimensions Returns the grid dimensions (number of blocks in x, y and z directions).
     * \return
     */
    virtual Dims Dimensions() = 0;

    /*!
     * \brief GetCell Get a cell from its global index.
     */
    virtual Cell GetCell(int global_index) = 0;

    /*!
     * \brief GetCell Get a cell from its (i,j,k) index.
     */
    virtual Cell GetCell(int i, int j, int k) = 0;

    /*!
     * \brief GetCell Get a cell from its (i,j,k) index.
     */
    virtual Cell GetCell(IJKCoordinate* ijk) = 0;

    /*!
     * \brief GetCellEnvelopingPoint Get the cell enveloping the point (x,y,z). Throws an exception if no cell is found.
     */
    virtual Cell GetCellEnvelopingPoint(double x, double y, double z) = 0;

    /*!
     * \brief GetCellEnvelopingPoint Get the cell enveloping the point (x,y,z). Throws an exception if no cell is found.
     */
    virtual Cell GetCellEnvelopingPoint(XYZCoordinate* xyz) = 0;

protected:
    GridSourceType type_;
    QString file_path_;
    Grid(GridSourceType type, QString file_path);
};

}
}
}

#endif // GRID_H
